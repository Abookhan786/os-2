%{
/*
 * ============================================================================
 * LEX PROGRAM: Arithmetic Calculator & Expression Evaluator
 * ============================================================================
 * Recognizes tokens in arithmetic expressions, maintains operand and operator
 * stacks, and computes the numerical result upon receiving a newline.
 *
 * Supported Operators:
 *   + (Addition)
 *   - (Subtraction)
 *   * (Multiplication)
 *   / (Division, with zero-division error handling)
 *   % (Modulus)
 *   ^ (Exponentiation)
 *   ( ) Parentheses for grouping
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAX_STACK 256

// Operand stack (double for floating-point precision)
static double num_stack[MAX_STACK];
static int num_top = -1;

// Operator stack
static char op_stack[MAX_STACK];
static int op_top = -1;

void push_num(double val) {
    if (num_top < MAX_STACK - 1) num_stack[++num_top] = val;
}

double pop_num() {
    if (num_top >= 0) return num_stack[num_top--];
    return 0.0;
}

void push_op(char op) {
    if (op_top < MAX_STACK - 1) op_stack[++op_top] = op;
}

char pop_op() {
    if (op_top >= 0) return op_stack[op_top--];
    return '\0';
}

char peek_op() {
    if (op_top >= 0) return op_stack[op_top];
    return '\0';
}

int precedence(char op) {
    switch (op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/':
        case '%': return 2;
        case '^': return 3;
        default:  return 0;
    }
}

int apply_op(char op) {
    if (num_top < 1) return 0; // Syntax error
    double b = pop_num();
    double a = pop_num();
    double res = 0.0;

    switch (op) {
        case '+': res = a + b; break;
        case '-': res = a - b; break;
        case '*': res = a * b; break;
        case '/':
            if (fabs(b) < 1e-9) {
                printf("Error: Division by zero!\n");
                return 0;
            }
            res = a / b;
            break;
        case '%':
            if ((long)b == 0) {
                printf("Error: Modulo by zero!\n");
                return 0;
            }
            res = (long)a % (long)b;
            break;
        case '^': res = pow(a, b); break;
        default: return 0;
    }
    push_num(res);
    return 1;
}

void evaluate_line() {
    while (op_top >= 0) {
        char op = pop_op();
        if (op == '(' || op == ')') {
            printf("Error: Mismatched parentheses!\n");
            num_top = -1;
            op_top = -1;
            return;
        }
        if (!apply_op(op)) {
            num_top = -1;
            op_top = -1;
            return;
        }
    }
    if (num_top == 0) {
        double ans = pop_num();
        if (floor(ans) == ans) {
            printf("Result = %.0f\n", ans);
        } else {
            printf("Result = %.4g\n", ans);
        }
    } else if (num_top > 0) {
        printf("Error: Invalid expression syntax!\n");
    }
    num_top = -1;
    op_top = -1;
}

%}

DIGIT       [0-9]
INT_CONST   {DIGIT}+
FLOAT_CONST {DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?|{DIGIT}+[eE][+-]?{DIGIT}+

%%

[ \t\r]+                            { /* Skip whitespace */ }

{FLOAT_CONST}                       {
                                        double val = atof(yytext);
                                        push_num(val);
                                    }

{INT_CONST}                         {
                                        double val = atof(yytext);
                                        push_num(val);
                                    }

"+"|"-"|"*"|"/"|"%"|"^"             {
                                        char current_op = yytext[0];
                                        while (op_top >= 0 && precedence(peek_op()) >= precedence(current_op) && current_op != '^') {
                                            if (!apply_op(pop_op())) break;
                                        }
                                        push_op(current_op);
                                    }

"("                                 {
                                        push_op('(');
                                    }

")"                                 {
                                        while (op_top >= 0 && peek_op() != '(') {
                                            if (!apply_op(pop_op())) break;
                                        }
                                        if (op_top >= 0 && peek_op() == '(') {
                                            pop_op(); // Discard '('
                                        } else {
                                            printf("Error: Unmatched right parenthesis ')'\n");
                                        }
                                    }

\n                                  {
                                        if (num_top >= 0 || op_top >= 0) {
                                            evaluate_line();
                                        }
                                        printf("\ncalc > ");
                                    }

.                                   {
                                        printf("Unknown character: '%s'\n", yytext);
                                    }

%%

int yywrap() {
    return 1;
}

int main() {
    printf("====================================================\n");
    printf("        LEX ARITHMETIC EXPRESSION CALCULATOR        \n");
    printf("====================================================\n");
    printf("Supported: +, -, *, /, %%, ^, (, ), integers, floats\n");
    printf("Enter expression (e.g. 10 + 4 * (8 - 3)) and press Enter:\n\ncalc > ");
    yylex();
    return 0;
}
