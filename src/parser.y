%{
#include "instructions.hpp"
#include "parser.tab.hpp"

#include <stdio.h>
#include <math.h>

#include <iostream>
#include <string>

int yylex();
int yyparse();
void yyerror(const char *s);


extern int yylineno;
extern bool local;
%}

%union {
    long long   llval;              // For numbers
    char*       strval;             // For identifiers
    int         address;
    Entity      *_entity;
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

%type <address> commands command condition
%type <strval> declarations identifier
%type <_entity> value expression 

%left '-' '+'
%left '*' '/'
%right '^'

%start program_all

%%

program_all : 
    procedures main {
        _put_halt();
    }
;

procedures :   
    procedures PROCEDURE proc_head IS declarations _BEGIN commands END {
        std::cout << "procedure\n";
        local = true;
        _put_rtrn();
    }
    | procedures PROCEDURE proc_head IS _BEGIN commands END {
        local = true;
        _put_rtrn();
    }
    | /* empty */
;

main : 
    PROGRAM IS declarations _BEGIN commands END {
        std::cout << "main\n";
        local = false;
    }
    | PROGRAM IS _BEGIN commands END {
        local = false;
    }
;

commands : 
    commands command {
        $$ = $2;
    }
    | command {
        $$ = $1;
    }
;

command :
    identifier ASSIGN expression ';' {
        $$ = _assign($1, $3);
    }
    | IF condition THEN commands ELSE commands ENDIF {
        //$$ = _if_else_stmt($2, $4, $6);
    }
    | IF condition THEN commands ENDIF {
        //$$ = _if_stmt($2, $4);
    }
    | WHILE condition DO commands ENDWHILE {
        //$$ = _while_stmt($2, $4);
    }
    | REPEAT commands UNTIL condition ';' {
        //$$ = _repeat_stmt($2, $4);
    }
    | FOR pidentifier FROM value TO value DO commands ENDFOR {
        //$$ = _for_stmt($2, $4, $6, $8);
    }
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
        //$$ = _for_dec_stmt($2, $4, $6, $8);
    }
    | proc_call ';'
    | READ identifier ';' {
        $$ = _read($2);
    }
    | WRITE value ';'{
        $$ = _write($2->address);
    }
;

proc_head :
    pidentifier '(' args_decl ')' {
        _procedure_head($1);
    }
;

proc_call :
    pidentifier '(' args ')' {

    }
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
    args_decl ',' pidentifier {
        _declare_arguments($3, SCALAR);
    }
    | args_decl ',' T pidentifier {
        _declare_arguments($4, ARRAY);
    }
    | pidentifier {
        _declare_arguments($1, SCALAR);
    }
    | T pidentifier {
        _declare_arguments($2, ARRAY);
    }
;

args :
    args ',' pidentifier
    | pidentifier
;

expression : 
    value { 
        $$ = $1;
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
    | value '%' value { 
        $$ = _mod($1, $3);
    }
;

condition :
    value EQ value {
        //$$ = _eq($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
    | value NEQ value {
        //$$ = _neq($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
    | value GT value { 
        //$$ = _gt($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
    | value LE value {
        //$$ = _le($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
    | value GEQ value {
        //$$ = _geq($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
    | value LEQ value {
        //$$ = _leq($1, $3); // Return pointer to the instruction with { JUMP -1 }
    }
;

value :
    num {
        $$ = new Entity($1, -1, "");
    }
    | '-' num {
        $$ = new Entity(-$2, -1, "");
    }
    | identifier {
        $$ = new Entity(-1, get_variable_address($1), $1);
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