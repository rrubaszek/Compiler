%{
#include "instructions.hpp"

#include <stdio.h>
#include <math.h>

#include <iostream>
#include <string>

int yylex();
int yyparse();
void yyerror(const char *s);

extern int yylineno;
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
        _halt();
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
        _assign($1);
       
    }
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition ';'
    | FOR pidentifier FROM value TO value DO commands ENDFOR
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
    | proc_call ';'
    | READ identifier ';' {
        _read($2); // TODO: Check examples to see if it is a correct approach
    }
    | WRITE value ';'{
        _write($2);
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
        _declare($3, SCALAR, 0, 0);
    }
    | declarations ',' pidentifier '[' num ':' num ']' { 
        _declare($3, ARRAY, $5, $7);
     }
    | pidentifier {
        _declare($1, SCALAR, 0, 0);
    }
    | pidentifier '[' num ':' num ']' { 
        _declare($1, ARRAY, $3, $5);
     }
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
        $$ = _load($1);
    }
    | value '+' value {
        $$ = _add($1, $3);
    }
    | value '-' value {
        $$ = _sub($1, $3);
    }
    | value '*' value { // Multiplication can be improved, get values by implementing memory and add bigger value, also use fast multiplication algorithm -> TODO: Implement memory
        $$ = _mul($1, $3);
    }
    | value '/' value { 
        $$ = _div($1, $3);
    }
    | value '%' value { /*TODO: implement _modulo($1, $3); */}
    | value HALF {
        $$ = _div2($1);
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
        $$ = _set($1);
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

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
}