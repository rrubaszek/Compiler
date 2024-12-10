%{
    #include "instructions.hpp"

    #include <stdio.h>

    #include <iostream>
    #include <string>
    #include <unordered_map>
    #include <vector>
    #include <utility>

    int yylex();
    int yyparse();
    void yyerror(const char *s);
    int get_variable_address(const std::string& name);

    extern int yylineno;

    int memory_address_counter = 1;

    std::vector<instruction> program;
    std::unordered_map<std::string, int> symbol_table;
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

%type <llval> value
%type <strval> expression declarations identifier

%left '-' '+'
%left '*' '/'
%right '^'

%start program_all

%%

program_all : 
    procedures main
;

procedures :   
    procedures PROCEDURE proc_head IS declarations _BEGIN commands END
    | procedures PROCEDURE proc_head IS _BEGIN commands END
    | /* empty */
;

main : 
    PROGRAM IS declarations _BEGIN commands END {
        program.emplace_back("HALT");
    }
    | PROGRAM IS _BEGIN commands END {
        program.emplace_back("HALT");
    }
;

commands : 
    commands command
    | command 
;

command :
    identifier ASSIGN expression ';' {
        program.emplace_back("STORE", get_variable_address($1));
    }
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition ';'
    | FOR pidentifier FROM value TO value DO commands ENDFOR
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
    | proc_call ';'
    | READ identifier ';'
    | WRITE value ';'{
        program.emplace_back("PUT", $2);
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
            symbol_table[$3] = memory_address_counter++;
            std::cout << "Dodano do pamięci zmienną: " << $3 << "\n";
        } 
        else {
            yyerror("Variable redeclaration");
        }
    }
    | declarations ',' pidentifier '[' num ':' num ']' { /*TODO: implement arrays*/ }
    | pidentifier {
        if (symbol_table.count($1) == 0) {
            symbol_table[$1] = memory_address_counter++;
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
    }
    | value '+' value {
        program.emplace_back("LOAD", $1);
        program.emplace_back("ADD", $3);
    }
    | value '-' value {
        program.emplace_back("LOAD", $1);
        program.emplace_back("SUB", $3);
    }
    | value '*' value { /*TODO: implement _multiply($1, $3); */}
    | value '/' value { /*TODO: implement _divide($1, $3); */}
    | value '%' value { /*TODO: implement _modulo($1, $3); */}
    | value HALF {
        program.emplace_back("HALF");
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
        int temp_register = memory_address_counter++;
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

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
}