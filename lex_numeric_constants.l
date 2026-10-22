%{
#include <stdio.h>
%}

DIGIT       [0-9]
INT         {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
SCIENTIFIC  {DIGIT}+(\.{DIGIT}+)?[eE][+-]?{DIGIT}+

%%
{SCIENTIFIC}    { printf("SCIENTIFIC: %s\n", yytext); }
{FLOAT}         { printf("FLOAT: %s\n", yytext); }
{INT}           { printf("INTEGER: %s\n", yytext); }
"+"             { printf("PLUS: %s\n", yytext); }
"-"             { printf("MINUS: %s\n", yytext); }
"*"             { printf("MULTIPLY: %s\n", yytext); }
"/"             { printf("DIVIDE: %s\n", yytext); }
"("             { printf("LPAREN: %s\n", yytext); }
")"             { printf("RPAREN: %s\n", yytext); }
[ \t\r\n]+      { ; }
.               { printf("INVALID: %s\n", yytext); }
%%

int yywrap() { return 1; }
int main() {
    yylex();
    return 0;
}
