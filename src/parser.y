%{
#include "instructions.hpp"
#include "Node.hpp"
#include "ArgsNode.hpp"
#include "CommandsNode.hpp"
#include "CommandNode.hpp"
#include "DeclarationsNode.hpp"
#include "ExpressionNode.hpp"
#include "IdentifierNode.hpp"
#include "MainNode.hpp"
#include "ValueNode.hpp"
#include "ProcedureNode.hpp"
#include "ProgramNode.hpp"

#include "parser.tab.hpp"

#include <stdio.h>
#include <math.h>

#include <iostream>
#include <string>
#include <utility>

int yylex();
int yyparse();
void yyerror(const char *s);


extern int yylineno;
extern bool local;
%}

%union {
    long long   llval;              // For numbers
    char*       strval;             // For identifiers
    MainNode*   main_node;
    ProcedureNode* procedure_node;
    CommandsNode*  commands_node;
    CommandNode*   command_node;
    DeclarationsNode* dec_node;
    ExpressionNode* expr_node;
    ValueNode*     num_node;
    IdentifierNode* id_node;
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

%type <main_node> main
%type <procedure_node> procedures proc_head
%type <dec_node> declarations
%type <commands_node> commands
%type <command_node> command
%type <expr_node> expression
%type <num_node> value
%type <id_node> identifier
 
%left '-' '+'
%left '*' '/'
%right '^'

%start program_all

%%

program_all : 
    procedures main {
        ProgramNode* node = new ProgramNode();
        
        if ($1 != nullptr)
            node->procedures.push_back($1);

        node->main = $2;
        node->compile();
        //_put_halt();
    }
;

procedures :   
    procedures PROCEDURE proc_head IS declarations _BEGIN commands END {
        ProcedureNode* node = new ProcedureNode();
        node->name = $3->name;
        node->args = $3->args;
        node->declarations = $5;
        node->commands = $7;
        $$ = node;
    }
    | procedures PROCEDURE proc_head IS _BEGIN commands END {
        ProcedureNode* node = new ProcedureNode();
        node->name = $3->name;
        node->args = $3->args;
        node->commands = $6;
        $$ = node;
    }
    | {
        $$ = nullptr;
    }
;

main : 
    PROGRAM IS declarations _BEGIN commands END {
        MainNode* node = new MainNode();
        node->declarations = $3;
        node->commands = $5;
        $$ = node;
    }
    | PROGRAM IS _BEGIN commands END {
        MainNode* node = new MainNode();
        node->commands = $4;
        $$ = node;
    }
;

commands : 
    commands command {
        $1->commands.push_back($2);
        $$ = $1;
    }
    | command {
        CommandsNode* node = new CommandsNode();
        node->commands.push_back($1);
        $$ = node;
    }
;

command :
    identifier ASSIGN expression ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::ASSIGN;
        $$ = node;
    }
    | IF condition THEN commands ELSE commands ENDIF {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::IF_ELSE;
        $$ = node;
    }
    | IF condition THEN commands ENDIF {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::IF;
        $$ = node;
    }
    | WHILE condition DO commands ENDWHILE {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::WHILE;
        $$ = node;
    }
    | REPEAT commands UNTIL condition ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::REPEAT;
        $$ = node;
    }
    | FOR pidentifier FROM value TO value DO commands ENDFOR {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::FOR;
        $$ = node;
    }
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::FOR_REV;
        $$ = node;
    }
    | proc_call ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::PROC_CALL;
        $$ = node;
    }
    | READ identifier ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::READ;
        $$ = node;
    }
    | WRITE value ';'{
        CommandNode* node = new CommandNode();
        node->type = CommandNode::WRITE;
        $$ = node;
    }
;

proc_head :
    pidentifier '(' args_decl ')' {
    }
;

proc_call :
    pidentifier '(' args ')' {
    }
;

declarations :
    declarations ',' pidentifier {
        $1->variables.push_back($3);
        $$ = $1;
    }
    | declarations ',' pidentifier '[' num ':' num ']' { 
        
    }
    | pidentifier {
        DeclarationsNode* node = new DeclarationsNode();
        node->variables.push_back($1);
        $$ = node;
    }
    | pidentifier '[' num ':' num ']' { 
        
    }
;

args_decl :
    args_decl ',' pidentifier {
        
    }
    | args_decl ',' T pidentifier {
        
    }
    | pidentifier {
        
    }
    | T pidentifier {
       
    }
;

args :
    args ',' pidentifier
    | pidentifier
;

expression : 
    value { 
        ExpressionNode* node = new ExpressionNode();
        //node->type = ExpressionNode::Number;
        //node->value = $1->value;
        $$ = node;
    }
    | value '+' value {
        //$$ = _add($1, $3);
    }
    | value '-' value {
        //$$ = _sub($1, $3);
    }
    | value '*' value { 
        //$$ = _mul($1, $3);
    }
    | value '/' value { 
       // $$ = _div($1, $3);
    }
    | value '%' value { 
        //$$ = _mod($1, $3);
    }
;

condition :
    value EQ value {
        //$$ = _eq($1, $3); // Return pointer to the instruction with { JUMP -1 }, 1 if always true, 0 if always false
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
        ValueNode* node = new ValueNode();
        node->value = $1;
        $$ = node;
    }
    | '-' num {
        ValueNode* node = new ValueNode();
        node->value = -$2;
        $$ = node;
    }
    | identifier {
        ValueNode* node = new ValueNode();
        node->name = $1->name;
        $$ = node;
    }
;

identifier :    
    pidentifier {
        IdentifierNode* node = new IdentifierNode();
        node->name = $1;
        $$ = node;
    }
    | pidentifier '[' pidentifier ']' {

    }
    | pidentifier '[' num ']' {

    }
;

%%

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
}