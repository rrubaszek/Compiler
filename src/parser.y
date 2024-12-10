%{
    #include <vector>
    #include <string>
    #include <unordered_map>
    #include <utility>
    #include <stdio.h>

    void yyerror(const char *s);
    int yylex();
    int yyparse();
    int get_variable_address(const std::string& name);

    extern int yylineno;

    int memory_address_counter = 1;

    struct instruction {
        std::string opcode;
        int operand;
    };

    std::vector<instruction> program;
    std::unordered_map<std::string, int> symbol_table;
%}

%union {
    long long   llval;              // For numbers
    char*       strval;             // For identifiers
    struct Array* arrval;    // For arrays
}

%token PROGRAM PROCEDURE IS _BEGIN END
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token FOR FROM TO DOWNTO ENDFOR
%token READ WRITE
%token T ASSIGN EQ LE GT GEQ LEQ NEQ 

%token <strval> pidentifier 
%token <llval> num

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
    PROGRAM IS declarations _BEGIN commands END
    | PROGRAM IS _BEGIN commands END
;

commands : 
    commands command
    | command 
;

command :
    identifier ASSIGN expression ';'
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition ';'
    | FOR pidentifier FROM value TO value DO commands ENDFOR
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
    | proc_call ';'
    | READ identifier ';'
    | WRITE value ';'
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
            symbol_table[$1] = memory_address_counter++;
        } 
        else {
            yyerror("Variable redeclaration");
        }
    }
    | declarations ',' pidentifier '[' num ':' num ']'
    | pidentifier
    | pidentifier '[' num ':' num ']'
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
        program.emplace_back({"LOAD", $1});
    }
    | value '+' value {
        program.emplace_back({"LOAD", $1});
        program.emplace_back({"ADD", $3});
    }
    | value '-' value {
        program.emplace_back({"LOAD", $1});
        program.emplace_back({"SUB", $3});
    }
    | value '*' value { /*TODO: implement _multiply($1, $3); */}
    | value '/' value { /*TODO: implement _divide($1, $3); */}
    | value '%' value { /*TODO: implement _modulo($1, $3); */}
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
        int temp_reg = memory_address_counter++;
        program.emplace_back({"SET", $1});          // SET immediate value
        program.emplace_back({"STORE", temp_reg});  // Store in temp memory
        $$ = temp_reg;
    }
    | identifier
;

identifier :    
    pidentifier {
        $$ = get_variable_address($1);
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