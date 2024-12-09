%{
    #include <stdio.h>

    void yyerror(const char *s);
    int yylex();
    int yyparse();

    extern int yylineno;

    struct ArrayInfo 
    {
        char* name;
        long long start;  
        long long end;
    };
%}

%union 
{
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
    declarations ',' pidentifier
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
    value
    | value '+' value
    | value '-' value
    | value '*' value
    | value '/' value
    | value '%' value
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
    num
    | identifier 
;

identifier :    
    pidentifier
    | pidentifier '[' pidentifier ']'
    | pidentifier '[' num ']'
;

%%

void yyerror(const char* s) 
{
	fprintf(stderr, "Parse error: %s\n", s);
}