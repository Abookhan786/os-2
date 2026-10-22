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
program: stmt_list { printf("Valid sequence of assignment statements\n"); return 0; }
       ;

stmt_list: stmt_list stmt
         | stmt
         ;

stmt: ID '=' expr ';'
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
    printf("Invalid sequence of assignment statements\n");
}

int main() {
    printf("Enter statements (Ctrl+Z or Ctrl+D to end):\n");
    yyparse();
    return 0;
}
