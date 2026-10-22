%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%token ID NUM AND OR NOT LT GT LE GE EQ NE
%left OR
%left AND
%left LT GT LE GE EQ NE
%right NOT

%%
input: expr { printf("Valid Boolean expression\n"); return 0; }
     ;

expr: expr OR expr
    | expr AND expr
    | expr LT expr
    | expr GT expr
    | expr LE expr
    | expr GE expr
    | expr EQ expr
    | expr NE expr
    | NOT expr
    | '(' expr ')'
    | ID
    | NUM
    ;
%%

void yyerror(const char *s) {
    printf("Invalid Boolean expression\n");
}

int main() {
    printf("Enter Boolean expression:\n");
    yyparse();
    return 0;
}
