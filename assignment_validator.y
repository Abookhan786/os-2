%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token ID NUM
%left '+' '-'
%left '*' '/'

%%
statement: ID '=' expr ';' { printf("Valid assignment statement\n"); return 0; }
         ;

expr: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | ID
    | NUM
    ;
%%

void yyerror(const char *s) {
    printf("Invalid assignment statement\n");
}

int main() {
    printf("Enter assignment statement:\n");
    yyparse();
    return 0;
}
