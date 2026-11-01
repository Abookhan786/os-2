/*
================================================================================
QUESTION: ARITHMETIC EXPRESSION CALCULATOR USING LEX
================================================================================
Problem Statement:
Write a Lex program to implement an Arithmetic Calculator that reads arithmetic
expressions containing numbers and operators, validates them, and computes the
evaluated numerical result.

The Calculator recognizes:
1. Operands:
   - Positive and negative integers: 10, 25, 100
   - Floating-point numbers: 3.14, 0.5, 2.718
2. Operators:
   - Addition (+)
   - Subtraction (-)
   - Multiplication (*)
   - Division (/) with zero-division detection
   - Modulus (%)
   - Exponentiation (^)
3. Grouping:
   - Parentheses: ( and )
4. Statement Terminator / Evaluation Trigger:
   - Newline (\n) computes and prints the final result.

Sample Inputs & Expected Outputs:
  Input  : 15 + 4 * 2
  Output : Result = 23

  Input  : (10 + 20) * 3 / 2
  Output : Result = 45

  Input  : 2 ^ 3 + 5 * 2
  Output : Result = 18

  Input  : 50 / 0
  Output : Error: Division by zero!

--------------------------------------------------------------------------------
COMPANION LEX SPECIFICATION (lex_calculator.l):
--------------------------------------------------------------------------------
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static double num_stack[256];
static int num_top = -1;
static char op_stack[256];
static int op_top = -1;

void push_num(double val) { num_stack[++num_top] = val; }
double pop_num() { return num_stack[num_top--]; }
void push_op(char op) { op_stack[++op_top] = op; }
char pop_op() { return op_stack[op_top--]; }
char peek_op() { return (op_top >= 0) ? op_stack[op_top] : '\0'; }

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^') return 3;
    return 0;
}

int apply_op(char op) {
    if (num_top < 1) return 0;
    double b = pop_num(), a = pop_num(), res = 0;
    if (op == '+') res = a + b;
    else if (op == '-') res = a - b;
    else if (op == '*') res = a * b;
    else if (op == '/') {
        if (fabs(b) < 1e-9) { printf("Error: Division by zero!\n"); return 0; }
        res = a / b;
    }
    else if (op == '%') res = (long)a % (long)b;
    else if (op == '^') res = pow(a, b);
    push_num(res);
    return 1;
}
%}

%%
[ \t\r]+                            ;
[0-9]+(\.[0-9]+)?                   { push_num(atof(yytext)); }
"+"|"-"|"*"|"/"|"%"|"^"             {
                                        while (op_top >= 0 && precedence(peek_op()) >= precedence(yytext[0]) && yytext[0] != '^')
                                            apply_op(pop_op());
                                        push_op(yytext[0]);
                                    }
"("                                 { push_op('('); }
")"                                 {
                                        while (op_top >= 0 && peek_op() != '(') apply_op(pop_op());
                                        if (op_top >= 0) pop_op();
                                    }
\n                                  {
                                        while (op_top >= 0) apply_op(pop_op());
                                        if (num_top == 0) printf("Result = %.4g\n", pop_num());
                                        num_top = op_top = -1;
                                    }
.                                   ;
%%
int yywrap() { return 1; }
int main() { yylex(); return 0; }
================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define STACK_CAPACITY 256

typedef struct {
    double data[STACK_CAPACITY];
    int top;
} NumStack;

typedef struct {
    char data[STACK_CAPACITY];
    int top;
} OpStack;

static void push_num(NumStack *s, double val) {
    if (s->top < STACK_CAPACITY - 1) s->data[++(s->top)] = val;
}

static double pop_num(NumStack *s) {
    if (s->top >= 0) return s->data[(s->top)--];
    return 0.0;
}

static void push_op(OpStack *s, char op) {
    if (s->top < STACK_CAPACITY - 1) s->data[++(s->top)] = op;
}

static char pop_op(OpStack *s) {
    if (s->top >= 0) return s->data[(s->top)--];
    return '\0';
}

static char peek_op(OpStack *s) {
    if (s->top >= 0) return s->data[s->top];
    return '\0';
}

static int get_precedence(char op) {
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

static int apply_operator(NumStack *nums, char op) {
    if (nums->top < 1) {
        printf("  [Error] Missing operand for operator '%c'\n", op);
        return 0;
    }
    double b = pop_num(nums);
    double a = pop_num(nums);
    double result = 0.0;

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (fabs(b) < 1e-9) {
                printf("  [Error] Division by zero!\n");
                return 0;
            }
            result = a / b;
            break;
        case '%':
            if ((long)b == 0) {
                printf("  [Error] Modulo by zero!\n");
                return 0;
            }
            result = (long)a % (long)b;
            break;
        case '^':
            result = pow(a, b);
            break;
        default:
            return 0;
    }
    push_num(nums, result);
    return 1;
}

// Evaluate an entire expression string using Lex-style scanner logic
void evaluate_expression(const char *expr) {
    NumStack nums = {.top = -1};
    OpStack ops = {.top = -1};

    int i = 0;
    int len = strlen(expr);
    int error = 0;
    int expecting_operand = 1; // Used to handle unary minus

    printf("Expression: %s\n", expr);

    while (i < len && !error) {
        // Skip whitespace
        if (isspace((unsigned char)expr[i])) {
            i++;
            continue;
        }

        // Handle Unary Minus or Negative Numbers
        if (expr[i] == '-' && expecting_operand) {
            int j = i + 1;
            while (j < len && isspace((unsigned char)expr[j])) j++;
            if (j < len && (isdigit((unsigned char)expr[j]) || expr[j] == '.')) {
                char *endptr;
                double val = strtod(&expr[i], &endptr);
                push_num(&nums, val);
                i = endptr - expr;
                expecting_operand = 0;
                continue;
            }
        }

        // Handle Numbers (Integers & Floats)
        if (isdigit((unsigned char)expr[i]) || expr[i] == '.') {
            char *endptr;
            double val = strtod(&expr[i], &endptr);
            push_num(&nums, val);
            i = endptr - expr;
            expecting_operand = 0;
            continue;
        }

        // Handle Left Parenthesis
        if (expr[i] == '(') {
            push_op(&ops, '(');
            expecting_operand = 1;
            i++;
            continue;
        }

        // Handle Right Parenthesis
        if (expr[i] == ')') {
            while (ops.top >= 0 && peek_op(&ops) != '(') {
                if (!apply_operator(&nums, pop_op(&ops))) {
                    error = 1;
                    break;
                }
            }
            if (ops.top >= 0 && peek_op(&ops) == '(') {
                pop_op(&ops); // Remove '('
            } else {
                printf("  [Error] Mismatched parentheses: unexpected ')'\n");
                error = 1;
            }
            expecting_operand = 0;
            i++;
            continue;
        }

        // Handle Binary Operators
        if (strchr("+-*/%^", expr[i])) {
            char current_op = expr[i];
            while (ops.top >= 0 && get_precedence(peek_op(&ops)) >= get_precedence(current_op) && current_op != '^') {
                if (!apply_operator(&nums, pop_op(&ops))) {
                    error = 1;
                    break;
                }
            }
            push_op(&ops, current_op);
            expecting_operand = 1;
            i++;
            continue;
        }

        printf("  [Error] Unrecognized character '%c'\n", expr[i]);
        error = 1;
        break;
    }

    // Apply remaining operators
    while (ops.top >= 0 && !error) {
        char op = pop_op(&ops);
        if (op == '(' || op == ')') {
            printf("  [Error] Mismatched parentheses in expression!\n");
            error = 1;
            break;
        }
        if (!apply_operator(&nums, op)) {
            error = 1;
            break;
        }
    }

    if (!error) {
        if (nums.top == 0) {
            double final_res = pop_num(&nums);
            if (floor(final_res) == final_res) {
                printf("  -> Result = %.0f\n\n", final_res);
            } else {
                printf("  -> Result = %.4g\n\n", final_res);
            }
        } else {
            printf("  [Error] Invalid syntax or extra operands!\n\n");
        }
    } else {
        printf("  -> Evaluation Failed.\n\n");
    }
}

int main(int argc, char *argv[]) {
    printf("=================================================================\n");
    printf("   CD LAB: Arithmetic Calculator (Lex Expression Evaluator)      \n");
    printf("=================================================================\n\n");

    if (argc > 1) {
        // Evaluate expression passed via command line
        evaluate_expression(argv[1]);
        return 0;
    }

    // Run standard test suite
    printf("--- Running Standard Test Cases ---\n");
    evaluate_expression("15 + 4 * 2");
    evaluate_expression("(10 + 20) * 3 / 2");
    evaluate_expression("2 ^ 3 + 5 * 2");
    evaluate_expression("100 - 25 * 3 + 10");
    evaluate_expression("50 / 0");
    evaluate_expression("2.5 * 4 + 1.25");

    // Interactive Mode
    printf("--- Interactive Calculator (Type 'exit' to quit) ---\n");
    char line[256];
    while (1) {
        printf("calc > ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) break;
        if (strlen(line) == 0) continue;
        evaluate_expression(line);
    }

    printf("Exiting calculator. Goodbye!\n");
    return 0;
}
