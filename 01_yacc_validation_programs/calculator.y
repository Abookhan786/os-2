%{
/*
 * ============================================================================
 * YACC GRAMMAR SPECIFICATION FOR DESKTOP CALCULATOR (calculator.y)
 * ============================================================================
 * Features:
 *   - Operator precedence and associativity:
 *       %left '+' '-'
 *       %left '*' '/' '%'
 *       %right '^'
 *       %nonassoc UMINUS (Unary minus)
 *   - Evaluation of arithmetic expressions into numerical results.
 *   - Division and modulus by zero runtime detection.
 *
 * Compilation with Lex & Yacc (Bison):
 *   yacc -d calculator.y
 *   lex calculator.l
 *   gcc -o calculator y.tab.c lex.yy.c -lm
 *
 * Execution:
 *   ./calculator
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int yylex();
void yyerror(const char *s);
%}

%union {
    double val;
}

%token <val> NUMBER
%type <val> expr

%left '+' '-'
%left '*' '/' '%'
%right '^'
%nonassoc UMINUS

%%

lines:
      /* empty */
    | lines line
    ;

line:
      '\n'
    | expr '\n' {
        if (floor($1) == $1) {
            printf("  = %.0f\n\ncalc > ", $1);
        } else {
            printf("  = %.4g\n\ncalc > ", $1);
        }
      }
    | error '\n' {
        yyerrok;
        printf("calc > ");
      }
    ;

expr:
      NUMBER             { $$ = $1; }
    | expr '+' expr      { $$ = $1 + $3; }
    | expr '-' expr      { $$ = $1 - $3; }
    | expr '*' expr      { $$ = $1 * $3; }
    | expr '/' expr      {
        if (fabs($3) < 1e-9) {
            yyerror("Division by zero");
            $$ = 0;
        } else {
            $$ = $1 / $3;
        }
      }
    | expr '%' expr      {
        if ((long)$3 == 0) {
            yyerror("Modulo by zero");
            $$ = 0;
        } else {
            $$ = (long)$1 % (long)$3;
        }
      }
    | expr '^' expr      { $$ = pow($1, $3); }
    | '-' expr %prec UMINUS { $$ = -$2; }
    | '(' expr ')'       { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    printf("====================================================\n");
    printf("     LEX & YACC ARITHMETIC DESKTOP CALCULATOR       \n");
    printf("====================================================\n");
    printf("Enter arithmetic expressions (e.g. (10 + 20) * 3 / 2):\n\ncalc > ");
    yyparse();
    return 0;
}
