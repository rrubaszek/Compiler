%option noyywrap
%option yylineno
%option nounput

%{
#include "parser.tab.hpp"

#include <string.h>
#include <stdlib.h>
%}

%%

\#.*\n                  {}
[ \t\r\n]+              {}
-?[0-9]+                { printf("%s\n", yytext); yylval.llval = atoll(yytext); return num; }
[_a-z]+                 { printf("%s\n", yytext); yylval.strval = strdup(yytext); return pidentifier; }
[+\-\*/^\()\[\]\;\,]    { printf("%c\n", yytext[0]); return yytext[0]; }
":="                    { printf("%s\n", yytext); return ASSIGN; }
"="                     { printf("%s\n", yytext); return EQ; }
"!="                    { printf("%s\n", yytext); return NEQ; }
"<"                     { printf("%s\n", yytext); return LE; }
">"                     { printf("%s\n", yytext); return GT; }
"<="                    { printf("%s\n", yytext); return LEQ; }
">="                    { printf("%s\n", yytext); return GEQ; }

PROGRAM             { printf("%s\n", yytext); return PROGRAM; }
PROCEDURE           { printf("%s\n", yytext); return PROCEDURE; }
IS                  { printf("%s\n", yytext); return IS; }
BEGIN               { printf("%s\n", yytext); return _BEGIN; }
END                 { printf("%s\n", yytext); return END; }
IF                  { printf("%s\n", yytext); return IF; }
THEN                { printf("%s\n", yytext); return THEN; }
ELSE                { printf("%s\n", yytext); return ELSE; }
ENDIF               { printf("%s\n", yytext); return ENDIF; }
WHILE               { printf("%s\n", yytext); return WHILE; }
DO                  { printf("%s\n", yytext); return DO; }
ENDWHILE            { printf("%s\n", yytext); return ENDWHILE; }
REPEAT              { printf("%s\n", yytext); return REPEAT; }
UNTIL               { printf("%s\n", yytext); return UNTIL; }
FOR                 { printf("%s\n", yytext); return FOR; }
FROM                { printf("%s\n", yytext); return FROM; }
TO                  { printf("%s\n", yytext); return TO; }
ENDFOR              { printf("%s\n", yytext); return ENDFOR; }
DOWNTO              { printf("%s\n", yytext); return DOWNTO; }
READ                { printf("%s\n", yytext); return READ; }
WRITE               { printf("%s\n", yytext); return WRITE; }
T                   { printf("%s\n", yytext); return T; }

%%
