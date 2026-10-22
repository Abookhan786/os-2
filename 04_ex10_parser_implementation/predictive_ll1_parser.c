/*
================================================================================
EXPERIMENT 10: PARSER IMPLEMENTATION (PREDICTIVE LL(1) PARSER)
================================================================================
AIM:
To study and implement a predictive parser (LL(1) parser) using a given grammar, 
construct the predictive parsing table using FIRST and FOLLOW sets, and verify 
the acceptance of valid input strings while identifying invalid strings.

INTRODUCTION:
A predictive parser is a top-down parser used in compiler design to determine 
whether an input string belongs to a given grammar. It uses a parsing table and a 
stack to select the appropriate production rules based on the current input symbol 
and the top of the stack. An LL(1) parser scans the input from left to right, 
produces a leftmost derivation, and uses one lookahead symbol to make parsing 
decisions.

Standard Arithmetic Grammar (Eliminating Left Recursion):
  1. E  -> T E'
  2. E' -> + T E'
  3. E' -> e (epsilon)
  4. T  -> F T'
  5. T' -> * F T'
  6. T' -> e (epsilon)
  7. F  -> ( E )
  8. F  -> id

FIRST Sets:
  FIRST(F)  = { (, id }
  FIRST(T') = { *, e }
  FIRST(T)  = { (, id }
  FIRST(E') = { +, e }
  FIRST(E)  = { (, id }

FOLLOW Sets:
  FOLLOW(E)  = { $, ) }
  FOLLOW(E') = { $, ) }
  FOLLOW(T)  = { +, $, ) }
  FOLLOW(T') = { +, $, ) }
  FOLLOW(F)  = { *, +, $, ) }

LL(1) Parsing Table M[A, a]:
-------------------------------------------------------------------------
Non-Term |      id      |      +       |      *       |      (       |      )       |      $
-------------------------------------------------------------------------
   E     |   E->TE'     |              |              |   E->TE'     |              |
   E'    |              |   E'->+TE'   |              |              |    E'->e     |    E'->e
   T     |   T->FT'     |              |              |   T->FT'     |              |
   T'    |              |    T'->e     |   T'->*FT'   |              |    T'->e     |    T'->e
   F     |    F->id     |              |              |   F->(E)     |              |
-------------------------------------------------------------------------
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK 100

/* Grammar Non-Terminals: E=0, E'=1, T=2, T'=3, F=4 */
/* Terminals: id=0, +=1, *=2, (=3, )=4, $=5 */

const char *non_terminals[] = {"E", "E'", "T", "T'", "F"};
const char *terminals[] = {"id", "+", "*", "(", ")", "$"};

/*
Production representations:
  0: (none)
  1: E -> T E'
  2: E' -> + T E'
  3: E' -> e
  4: T -> F T'
  5: T' -> * F T'
  6: T' -> e
  7: F -> ( E )
  8: F -> id
*/
const char *production_str[] = {
    "Error",
    "E -> T E'",
    "E' -> + T E'",
    "E' -> e",
    "T -> F T'",
    "T' -> * F T'",
    "T' -> e",
    "F -> ( E )",
    "F -> id"
};

int parsing_table[5][6] = {
    /*          id    +    *    (    )    $   */
    /* E  */  {  1,   0,   0,   1,   0,   0 },
    /* E' */  {  0,   2,   0,   0,   3,   3 },
    /* T  */  {  4,   0,   0,   4,   0,   0 },
    /* T' */  {  0,   6,   5,   0,   6,   6 },
    /* F  */  {  8,   0,   0,   7,   0,   0 }
};

int get_terminal_index(const char *tok) {
    if (strcmp(tok, "id") == 0) return 0;
    if (strcmp(tok, "+") == 0) return 1;
    if (strcmp(tok, "*") == 0) return 2;
    if (strcmp(tok, "(") == 0) return 3;
    if (strcmp(tok, ")") == 0) return 4;
    if (strcmp(tok, "$") == 0) return 5;
    return -1;
}

/* Parse table-driven LL(1) simulator */
int parse_string(const char *input_tokens[], int num_tokens) {
    char stack[MAX_STACK][8];
    int top = 0;

    // Push initial markers: $ and start symbol E
    strcpy(stack[top++], "$");
    strcpy(stack[top++], "E");

    int ip = 0;

    printf("\n%-25s %-25s %-25s\n", "Stack", "Input", "Action");
    printf("-------------------------------------------------------------------------\n");

    while (top > 0) {
        // Print stack
        char stack_str[128] = "";
        for (int i = 0; i < top; i++) {
            strcat(stack_str, stack[i]);
            strcat(stack_str, " ");
        }

        // Print remaining input
        char input_str[128] = "";
        for (int i = ip; i < num_tokens; i++) {
            strcat(input_str, input_tokens[i]);
            strcat(input_str, " ");
        }

        char *X = stack[top - 1];
        const char *a = input_tokens[ip];
        int a_idx = get_terminal_index(a);

        if (a_idx == -1) {
            printf("%-25s %-25s Error: Invalid token '%s'\n", stack_str, input_str, a);
            return 0;
        }

        // Check if X is terminal or '$'
        int is_terminal = 0;
        for (int t = 0; t < 6; t++) {
            if (strcmp(X, terminals[t]) == 0) {
                is_terminal = 1;
                break;
            }
        }

        if (is_terminal) {
            if (strcmp(X, a) == 0) {
                char action[64];
                sprintf(action, "Match %s", a);
                printf("%-25s %-25s %-25s\n", stack_str, input_str, action);
                top--; // pop X
                ip++;  // advance input
                if (strcmp(X, "$") == 0) {
                    return 1; // accepted
                }
            } else {
                printf("%-25s %-25s Error: Mismatch, expected '%s' got '%s'\n", stack_str, input_str, X, a);
                return 0;
            }
        } else {
            // X is non-terminal
            int nt_idx = -1;
            for (int nt = 0; nt < 5; nt++) {
                if (strcmp(X, non_terminals[nt]) == 0) {
                    nt_idx = nt;
                    break;
                }
            }

            if (nt_idx == -1) {
                printf("%-25s %-25s Error: Unknown symbol '%s'\n", stack_str, input_str, X);
                return 0;
            }

            int rule = parsing_table[nt_idx][a_idx];
            if (rule == 0) {
                printf("%-25s %-25s Error: No rule in M[%s, %s]\n", stack_str, input_str, X, a);
                return 0;
            }

            printf("%-25s %-25s Output: %s\n", stack_str, input_str, production_str[rule]);
            top--; // pop non-terminal

            // Push right-hand side in reverse order
            switch (rule) {
                case 1: // E -> T E'
                    strcpy(stack[top++], "E'");
                    strcpy(stack[top++], "T");
                    break;
                case 2: // E' -> + T E'
                    strcpy(stack[top++], "E'");
                    strcpy(stack[top++], "T");
                    strcpy(stack[top++], "+");
                    break;
                case 3: // E' -> e (nothing pushed)
                    break;
                case 4: // T -> F T'
                    strcpy(stack[top++], "T'");
                    strcpy(stack[top++], "F");
                    break;
                case 5: // T' -> * F T'
                    strcpy(stack[top++], "T'");
                    strcpy(stack[top++], "F");
                    strcpy(stack[top++], "*");
                    break;
                case 6: // T' -> e (nothing pushed)
                    break;
                case 7: // F -> ( E )
                    strcpy(stack[top++], ")");
                    strcpy(stack[top++], "E");
                    strcpy(stack[top++], "(");
                    break;
                case 8: // F -> id
                    strcpy(stack[top++], "id");
                    break;
            }
        }
    }

    return 0;
}

int main(void) {
    printf("=========================================================================\n");
    printf("         LL(1) PREDICTIVE PARSER IMPLEMENTATION (EX NO: 10)              \n");
    printf("=========================================================================\n");
    printf("Grammar:\n");
    printf("  E  -> T E'\n");
    printf("  E' -> + T E' | e\n");
    printf("  T  -> F T'\n");
    printf("  T' -> * F T' | e\n");
    printf("  F  -> ( E ) | id\n\n");

    printf("Predictive Parsing Table M[A, a]:\n");
    printf("%-8s | %-10s %-10s %-10s %-10s %-10s %-10s\n", "NT", "id", "+", "*", "(", ")", "$");
    printf("-------------------------------------------------------------------------\n");
    for (int i = 0; i < 5; i++) {
        printf("%-8s | ", non_terminals[i]);
        for (int j = 0; j < 6; j++) {
            if (parsing_table[i][j] != 0) {
                printf("%-10s ", production_str[parsing_table[i][j]]);
            } else {
                printf("%-10s ", "--");
            }
        }
        printf("\n");
    }

    // Test with sample valid input: id + id * id $
    const char *test1[] = {"id", "+", "id", "*", "id", "$"};
    printf("\n>>> Parsing Valid Input: id + id * id $\n");
    if (parse_string(test1, 6)) {
        printf("\nResult: String ACCEPTED by LL(1) Predictive Parser!\n");
    } else {
        printf("\nResult: String REJECTED!\n");
    }

    // Test with sample invalid input: id + * id $
    const char *test2[] = {"id", "+", "*", "id", "$"};
    printf("\n>>> Parsing Invalid Input: id + * id $\n");
    if (parse_string(test2, 5)) {
        printf("\nResult: String ACCEPTED!\n");
    } else {
        printf("\nResult: String REJECTED by LL(1) Predictive Parser!\n");
    }

    return 0;
}
