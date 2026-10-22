%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token NUM
%left '+' '-'
%left '*' '/'

%%
input: expr { printf("Valid arithmetic expression\n"); return 0; }
     ;

expr: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | NUM
    ;
%%

void yyerror(const char *s) {
    printf("Invalid arithmetic expression\n");
}

int main() {
    printf("Enter expression:\n");
    yyparse();
    return 0;
}
