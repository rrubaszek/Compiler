%{
#include "instructions.hpp"
#include "Node.hpp"
#include "ArgsNode.hpp"
#include "CommandsNode.hpp"
#include "CommandNode.hpp"
#include "ConditionNode.hpp"
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

std::vector<ProcedureNode*> temporary_procedures;

extern int yylineno;
%}

%union {
    long long   llval;              // For numbers
    char*       strval;             // For identifiers
    std::vector<std::string>*   _args;
    std::vector<std::pair<std::string, bool>>*  _args_decl;
    MainNode*   main_node;
    ProcedureNode* procedure_node;
    CommandsNode*  commands_node;
    CommandNode*   command_node;
    ConditionNode* cond_node;
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
%type <command_node> command proc_call
%type <expr_node> expression
%type <cond_node> condition
%type <num_node> value
%type <id_node> identifier
%type <_args_decl> args_decl
%type <_args> args
 
%left '-' '+'
%left '*' '/'
%right '^'

%start program_all

%%

program_all : 
    procedures main {
        ProgramNode* node = new ProgramNode();
        
        if ($1 != nullptr)
            node->procedures = std::move(temporary_procedures);

        node->main = $2;
        node->compile();
    }
;

procedures :   
    procedures PROCEDURE proc_head IS declarations _BEGIN commands END {
        $3->declarations = $5;
        $3->commands = $7;
        temporary_procedures.push_back($3);
        $$ = $3;
    }
    | procedures PROCEDURE proc_head IS _BEGIN commands END {
        $3->commands = $6;
        temporary_procedures.push_back($3);
        $$ = $3;
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

command:
    identifier ASSIGN expression ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::ASSIGN;
        CommandNode::AssignData data;
        data.left = $1;  
        data.right = $3; 
        node->data = data;
        $$ = node;
    }
    | IF condition THEN commands ELSE commands ENDIF {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::IF_ELSE;
        CommandNode::IfElseData data;
        data.condition = $2;   
        data.then_branch = $4;
        data.else_branch = $6; 
        node->data = data;
        $$ = node;
    }
    | IF condition THEN commands ENDIF {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::IF;
        CommandNode::IfData data;
        data.condition = $2;    
        data.then_branch = $4; 
        node->data = data;
        $$ = node;
    }
    | WHILE condition DO commands ENDWHILE {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::WHILE;
        CommandNode::WhileData data;
        data.condition = $2;    
        data.loop_body = $4; 
        node->data = data;
        $$ = node;
    }
    | REPEAT commands UNTIL condition ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::REPEAT;
        CommandNode::RepeatData data;
        data.loop_body = $2;  
        data.condition = $4; 
        node->data = data;
        $$ = node;
    }
    | FOR pidentifier FROM value TO value DO commands ENDFOR {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::FOR;
        CommandNode::ForData data;
        data.loop_variable = $2;   
        data.start_value = $4;    
        data.end_value = $6;     
        data.loop_body = $8;     
        node->data = data;
        $$ = node;
    }
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::FOR_REV;
        CommandNode::ForRevData data;
        data.loop_variable = std::string($2);   
        data.start_value = $4;    
        data.end_value = $6;     
        data.loop_body = $8;     
        node->data = data;
        $$ = node;
    }
    | proc_call ';' {    
        $$ = $1;
    }
    | READ identifier ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::READ;
        CommandNode::ReadData data;
        data.target = $2; 
        node->data = data;
        $$ = node;
    }
    | WRITE value ';' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::WRITE;
        CommandNode::WriteData data;
        data.value = $2;
        node->data = data;
        $$ = node;
    }
;

proc_head :
    pidentifier '(' args_decl ')' {
        ProcedureNode* node = new ProcedureNode();
        node->name = $1;               
        node->args = std::move(*$3);               
        $$ = node; 
    }
;

proc_call :
    pidentifier '(' args ')' {
        CommandNode* node = new CommandNode();
        node->type = CommandNode::PROC_CALL;  
        CommandNode::ProcCallData data;
        data.name = $1;           
        data.args = std::move(*$3);           
        node->data = data;
        $$ = node;
    }
;

declarations :
    declarations ',' pidentifier {
        $1->variables.push_back($3);
        $$ = $1;
    }
    | declarations ',' pidentifier '[' num ':' num ']' { 
        DeclarationsNode::ArrayDeclarations array;
        array.name = $3;
        array.start = $5;
        array.end = $7;
        $1->arrays.push_back(array);
        $$ = $1;
    }
    | pidentifier {
        DeclarationsNode* node = new DeclarationsNode();
        node->variables.push_back($1);
        $$ = node;
    }
    | pidentifier '[' num ':' num ']' { 
        DeclarationsNode* node = new DeclarationsNode();
        DeclarationsNode::ArrayDeclarations array;
        array.name = $1;
        array.start = $3;
        array.end = $5;
        node->arrays.push_back(array);
        $$ = node;
    }
;

args_decl :
    args_decl ',' pidentifier {
        $1->push_back(std::make_pair($3, false)); 
        $$ = $1;
    }
    | args_decl ',' T pidentifier {
        DeclarationsNode::ArrayDeclarations node;
        node.name = $4;
        $1->push_back(std::make_pair($4, true)); 
        $$ = $1;
    }
    | pidentifier {
        std::vector<std::pair<std::string, bool>>* args = new std::vector<std::pair<std::string, bool>>();
        args->push_back(std::make_pair($1, false)); 
        $$ = args;
    }
    | T pidentifier {
        std::vector<std::pair<std::string, bool>>* args = new std::vector<std::pair<std::string, bool>>();
        args->push_back(std::make_pair($2, true));
        $$ = args;
    }
;

args :
    args ',' pidentifier {
        $1->push_back($3);
        $$ = $1;
    }
    | pidentifier {
        std::vector<std::string>* args = new std::vector<std::string>();
        args->push_back($1);
        $$ = args;
    }
;

expression : 
    value { 
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::VALUE;
        node->left = $1;
        $$ = node;
    }
    | value '+' value {
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::ADD;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value '-' value {
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::SUB;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value '*' value { 
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::MUL;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value '/' value { 
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::DIV;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value '%' value { 
        ExpressionNode* node = new ExpressionNode();
        node->type = ExpressionNode::ExpressionType::MOD;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
;

condition :
    value EQ value {
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::EQ;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value NEQ value {
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::NEQ;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value GT value { 
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::GT;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value LE value {
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::LT;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value GEQ value {
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::GEQ;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
    | value LEQ value {
        ConditionNode* node = new ConditionNode();
        node->type = ConditionNode::ConditionType::LEQ;
        node->left = $1;
        node->right = $3;
        $$ = node;
    }
;

value :
    num {
        ValueNode* node = new ValueNode();
        node->value = $1;
        node->type = ValueNode::ValueType::CONSTANT;
        $$ = node;
    }
    | '-' num {
        ValueNode* node = new ValueNode();
        node->value = -$2;
        node->type = ValueNode::ValueType::CONSTANT;
        $$ = node;
    }
    | identifier {
        ValueNode* node = new ValueNode();
        node->name = $1->name;
        node->type = ValueNode::ValueType::VARIABLE;

        if ($1->is_array) {
            node->type = ValueNode::ValueType::ARRAY_ELEMENT;
            node->index_name = $1->index_name;
            node->index_value = $1->index_value;
        }
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
        IdentifierNode* node = new IdentifierNode();
        node->name = $1;
        node->is_array = true;
        node->index_name = $3;
        $$ = node;
    }
    | pidentifier '[' num ']' {
        IdentifierNode* node = new IdentifierNode();
        node->name = $1;
        node->is_array = true;
        node->index_value = $3;
        $$ = node;
    }
;

%%

void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
}