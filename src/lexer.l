%option noyywrap
%option nounput
%option yylineno

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

#include <string.h>
#include <stdlib.h>

#include <iostream>
%}

%%

\#.*\n                {}
[ \t\r\n]+            {}
[_a-z]+               { yylval.strval = strdup(yytext); return pidentifier; }
[0-9]+                { yylval.llval = atoll(yytext); return num; }
[+\-\*/^\()\[\]\;\:\,\%]    { return yytext[0]; }

":="                  { return ASSIGN; }
"="                   { return EQ; }
"!="                  { return NEQ; }
"<"                   { return LE; }
">"                   { return GT; }
"<="                  { return LEQ; }
">="                  { return GEQ; }

"PROGRAM"             { return PROGRAM; }
"PROCEDURE"           { return PROCEDURE; }
"IS"                  { return IS; }
"BEGIN"               { return _BEGIN; }
"END"                 { return END; }
"IF"                  { return IF; }
"THEN"                { return THEN; }
"ELSE"                { return ELSE; }
"ENDIF"               { return ENDIF; }
"WHILE"               { return WHILE; }
"DO"                  { return DO; }
"ENDWHILE"            { return ENDWHILE; }
"REPEAT"              { return REPEAT; }
"UNTIL"               { return UNTIL; }
"FOR"                 { return FOR; }
"FROM"                { return FROM; }
"TO"                  { return TO; }
"ENDFOR"              { return ENDFOR; }
"DOWNTO"              { return DOWNTO; }
"READ"                { return READ; }
"WRITE"               { return WRITE; }
"T"                   { return T; }

%%
