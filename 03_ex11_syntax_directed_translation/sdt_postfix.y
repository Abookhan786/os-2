%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%union { char *str; }
%token <str> ID NUM
%left '+' '-'
%left '*' '/'

%%
input: expr { printf("\n"); }
     ;

expr: expr '+' expr     { printf("+"); }
    | expr '-' expr     { printf("-"); }
    | expr '*' expr     { printf("*"); }
    | expr '/' expr     { printf("/"); }
    | '(' expr ')'
    | ID                { printf("%s", $1); free($1); }
    | NUM               { printf("%s", $1); free($1); }
    ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: Invalid expression\n");
}

int main() {
    printf("Enter infix expression: ");
    yyparse();
    return 0;
}
