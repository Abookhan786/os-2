%{
#include <stdio.h>
%}

%%
[ \t\r\n]+                          { ; }
";"                                 { printf("SEMICOLON: ;\n"); }
"="                                 { printf("ASSIGN: =\n"); }
"+"                                 { printf("PLUS: +\n"); }
"-"                                 { printf("MINUS: -\n"); }
"*"                                 { printf("MULTIPLY: *\n"); }
"/"                                 { printf("DIVIDE: /\n"); }
"("                                 { printf("LPAREN: (\n"); }
")"                                 { printf("RPAREN: )\n"); }
[0-9]+\.[0-9]+                      { printf("FLOAT: %s\n", yytext); }
[0-9]+                              { printf("INTEGER: %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*              { printf("IDENTIFIER: %s\n", yytext); }
.                                   { printf("UNKNOWN: %s\n", yytext); }
%%

int yywrap() { return 1; }
int main() {
    yylex();
    return 0;
}
