%{
#include <stdio.h>
%}

%%
[ \t\r\n]+                          { ; }
"&&"                                { printf("AND: %s\n", yytext); }
"||"                                { printf("OR: %s\n", yytext); }
"!"                                 { printf("NOT: %s\n", yytext); }
"<="|">="|"=="|"!="|"<"|">"         { printf("RELATIONAL_OPERATOR: %s\n", yytext); }
"("                                 { printf("LPAREN: %s\n", yytext); }
")"                                 { printf("RPAREN: %s\n", yytext); }
[0-9]+                              { printf("INTEGER: %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*              { printf("IDENTIFIER: %s\n", yytext); }
.                                   { printf("UNKNOWN: %s\n", yytext); }
%%

int yywrap() { return 1; }
int main() {
    yylex();
    return 0;
}
