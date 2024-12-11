%{
    #include "instructions.hpp"

    #include <stdio.h>
    #include <math.h>

    #include <iostream>
    #include <string>
    #include <stack>
    #include <vector>
    #include <utility>
    #include <unordered_map>

    int yylex();
    int yyparse();
    void yyerror(const char *s);
    int get_variable_address(const std::string& name);
    int get_variable_value(const int& address);

    int allocate_register();
    void free_register(int reg);

    extern int yylineno;

    int memory_address_counter = 1;

    std::vector<instruction> program;
    std::unordered_map<std::string, int> symbol_table; // each symbol has its register
    std::unordered_map<int, long long> memory; // each register hold a value
    std::stack<int> free_registers; // Stack for reusing temporary registers
    int next_free_register = 1; // Tracks the next available memory address
%}

%union {
    long long   llval;              // For numbers
    char*       strval;             // For identifiers
}

%token PROGRAM PROCEDURE IS _BEGIN END
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token FOR FROM TO DOWNTO ENDFOR
%token READ WRITE
%token T ASSIGN EQ LE GT GEQ LEQ NEQ HALF
%token <strval> pidentifier 
%token <llval> num

%type <llval> value expression
%type <strval> declarations identifier

%left '-' '+'
%left '*' '/'
%right '^'

%start program_all

%%

program_all : 
    procedures main {
        program.emplace_back("HALT");
    }
;

procedures :   
    procedures PROCEDURE proc_head IS declarations _BEGIN commands END
    | procedures PROCEDURE proc_head IS _BEGIN commands END
    | /* empty */
;

main : 
    PROGRAM IS declarations _BEGIN commands END
    | PROGRAM IS _BEGIN commands END 
;

commands : 
    commands command
    | command 
;

command :
    identifier ASSIGN expression ';' {
        program.emplace_back("STORE", get_variable_address($1));
        free_register($3);
    }
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition ';'
    | FOR pidentifier FROM value TO value DO commands ENDFOR
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
    | proc_call ';'
    | READ identifier ';' {
        program.emplace_back("GET", get_variable_address($2)); // TODO: Check examples to see if it is a correct approach
    }
    | WRITE value ';'{
        program.emplace_back("PUT", $2);
        free_register($2);
    }
;

proc_head :
    pidentifier '(' args_decl ')'
;

proc_call :
    pidentifier '(' args ')'
;

declarations :
    declarations ',' pidentifier {
        if (symbol_table.count($3) == 0) {
            symbol_table[$3] = allocate_register();
            std::cout << "Dodano do pamięci zmienną: " << $3 << "\n";
        }
        else {
            yyerror("Variable redeclaration");
        }
    }
    | declarations ',' pidentifier '[' num ':' num ']' { /*TODO: implement arrays*/ }
    | pidentifier {
        if (symbol_table.count($1) == 0) {
            symbol_table[$1] = allocate_register();
            std::cout << "Dodano do pamięci zmienną: " << $1 << "\n";
        } 
        else {
            yyerror("Variable redeclaration");
        }
    }
    | pidentifier '[' num ':' num ']' { /*TODO: implement arrays */ }
;

args_decl :
    args_decl ',' pidentifier
    | args_decl ',' T pidentifier
    | pidentifier
    | T pidentifier
;

args :
    args ',' pidentifier
    | pidentifier
;

expression :
    value { 
        program.emplace_back("LOAD", $1);
        $$ = $1;
    }
    | value '+' value {
        program.emplace_back("LOAD", $1);
        program.emplace_back("ADD", $3);
        free_register($3);
        $$ = $1;
    }
    | value '-' value {
        program.emplace_back("LOAD", $1);
        program.emplace_back("SUB", $3);
        free_register($3);
        $$ = $1;
    }
    | value '*' value { // Multiplication can be improved, get values by implementing memory and add bigger value, also use fast multiplication algorithm -> TODO: Implement memory
        int res_addr = allocate_register();

        program.emplace_back("SET", 0);
        program.emplace_back("STORE", res_addr);

        program.emplace_back("LOAD", res_addr);
        program.emplace_back("ADD", $1);
        program.emplace_back("STORE", res_addr);

        program.emplace_back("SET", -1); 
        program.emplace_back("ADD", $3);
        program.emplace_back("STORE", $3);
        program.emplace_back("JPOS", -6);

        program.emplace_back("LOAD", res_addr);

        free_register(res_addr);
        free_register($3);

        $$ = $1;
    }
    | value '/' value { 
        int numerator = $1; 
        int denominator = $3; 

        int quotient_reg = allocate_register(); 
        int remainder_reg = allocate_register(); 

        program.emplace_back("SET", 0); 
        program.emplace_back("STORE", quotient_reg);

        program.emplace_back("LOAD", numerator); 
        program.emplace_back("STORE", remainder_reg);

        program.emplace_back("LOAD", remainder_reg); 
        program.emplace_back("SUB", denominator);
        program.emplace_back("JNEG", 8); // Exit loop 

        program.emplace_back("SET", 1);
        program.emplace_back("ADD", quotient_reg);
        program.emplace_back("STORE", quotient_reg);

        program.emplace_back("LOAD", remainder_reg);
        program.emplace_back("SUB", denominator);
        program.emplace_back("STORE", remainder_reg);

        program.emplace_back("JUMP", -9);

        program.emplace_back("LOAD", quotient_reg); 

        free_register(remainder_reg);
        free_register(quotient_reg);

        $$ = quotient_reg;
    }
    | value '%' value { /*TODO: implement _modulo($1, $3); */}
    | value HALF {
        program.emplace_back("HALF");
        $$ = $1;
    }
;

condition :
    value EQ value
    | value NEQ value
    | value GT value
    | value LE value
    | value GEQ value
    | value LEQ value
;

value :
    num {
        int temp_register = allocate_register();
        program.emplace_back("SET", $1);
        program.emplace_back("STORE", temp_register);
        $$ = temp_register;
    }
    | identifier {
        $$ = get_variable_address($1);
    }
;

identifier :    
    pidentifier {
        $$ = $1;
    }
    | pidentifier '[' pidentifier ']'
    | pidentifier '[' num ']'
;

%%

int get_variable_address(const std::string& name) {
    if (symbol_table.count(name) == 0) {
        yyerror(("Undefined variable: " + name).c_str());
        exit(1);
    }
    return symbol_table[name];
}

int get_variable_value(const int& address) {
    if (memory.find(address) == memory.end()) {
        yyerror("This address in memory is empty");
        exit(1);
    }
    else {
        return memory[address];
    }
}

void allocate_memory(int address, int value) {
    memory[address] = value;
}

int allocate_register() {
    if (!free_registers.empty()) {
        int reg = free_registers.top();
        free_registers.pop();
        std::cout << "register allocated " << reg << "\n";
        return reg;
    }
    return next_free_register++;
}

void free_register(int reg) {
    std::cout << "free register " << reg << "\n";
    free_registers.push(reg);
}

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
}