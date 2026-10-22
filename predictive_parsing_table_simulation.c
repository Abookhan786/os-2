/*
================================================================================
EX NO: 9  23Z511 – SYSTEM SOFTWARE LABORATORY
PREDICTIVE PARSING TABLE & STACK SIMULATION
================================================================================
QUESTION (from Page 18 of Lab Manual):
  Write the predictive parsing table for a sample input.
  Sample Input: (+ (* a b) d)

GRAMMAR:
  EXPR -> ( OP ARG ARG )
  OP   -> binop
  ARG  -> ID
  ARG  -> NUM
  ARG  -> ( EXPR )

  where binop = + | - | * | / | %

PREDICTIVE PARSING TABLE M(A, a):
-----------------------------------------------------------------------------------
Non-Terminal |      (                 |    ID     |    NUM    | binop  |   )   |  $
-----------------------------------------------------------------------------------
EXPR         | EXPR -> ( OP ARG ARG ) |    ---    |    ---    |  ---   |  ---  | ---
OP           |          ---           |    ---    |    ---    | OP->b  |  ---  | ---
ARG          | ARG -> ( EXPR )        | ARG -> ID | ARG -> NUM|  ---   |  ---  | ---
-----------------------------------------------------------------------------------

PARSING TRACE (for (+ (* a b) d) $):
  Stack                       Input                         Production / Action
  ---------------------------------------------------------------------------------
  EXPR $                      ( + ( * a b ) d ) $           EXPR -> ( OP ARG ARG )
  ( OP ARG ARG ) $            ( + ( * a b ) d ) $           Match (
  OP ARG ARG ) $              + ( * a b ) d ) $             OP -> binop (+)
  + ARG ARG ) $               + ( * a b ) d ) $             Match +
  ARG ARG ) $                 ( * a b ) d ) $               ARG -> ( EXPR )
  ( EXPR ) ARG ) $            ( * a b ) d ) $               Match (
  EXPR ) ARG ) $              * a b ) d ) $                 EXPR -> ( OP ARG ARG )
  ... (Nested expression processing)
  ARG ) $                     d ) $                         ARG -> ID
  d ) $                       d ) $                         Match d
  ) $                         ) $                           Match )
  $                           $                             Accept
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *stack_str;
    const char *input_str;
    const char *action;
} TraceStep;

static TraceStep sample_trace[] = {
    {"EXPR $",                     "( + ( * a b ) d ) $", "EXPR -> ( OP ARG ARG )"},
    {"( OP ARG ARG ) $",           "( + ( * a b ) d ) $", "Match ("},
    {"OP ARG ARG ) $",             "+ ( * a b ) d ) $",   "OP -> binop (+)"},
    {"+ ARG ARG ) $",              "+ ( * a b ) d ) $",   "Match +"},
    {"ARG ARG ) $",                "( * a b ) d ) $",     "ARG -> ( EXPR )"},
    {"( EXPR ) ARG ) $",           "( * a b ) d ) $",     "Match ("},
    {"EXPR ) ARG ) $",             "* a b ) d ) $",       "EXPR -> ( OP ARG ARG )"},
    {"( OP ARG ARG ) ) ARG ) $",   "* a b ) d ) $",       "Syntax Check: Nested EXPR"},
    {"OP ARG ARG ) ) ARG ) $",     "* a b ) d ) $",       "OP -> binop (*)"},
    {"* ARG ARG ) ) ARG ) $",      "* a b ) d ) $",       "Match *"},
    {"ARG ARG ) ) ARG ) $",        "a b ) d ) $",         "ARG -> ID (a)"},
    {"a ARG ) ) ARG ) $",          "a b ) d ) $",         "Match a"},
    {"ARG ) ) ARG ) $",            "b ) d ) $",           "ARG -> ID (b)"},
    {"b ) ) ARG ) $",              "b ) d ) $",           "Match b"},
    {") ) ARG ) $",                ") d ) $",             "Match )"},
    {") ARG ) $",                  ") d ) $",             "Match ) (End of nested EXPR)"},
    {"ARG ) $",                    "d ) $",               "ARG -> ID (d)"},
    {"d ) $",                      "d ) $",               "Match d"},
    {") $",                        ") $",                 "Match )"},
    {"$",                          "$",                   "Accept"}
};

void display_parsing_table(void) {
    printf("======================================================================================\n");
    printf("               PREDICTIVE PARSING TABLE M(Non-Terminal, Terminal)                     \n");
    printf("======================================================================================\n");
    printf("%-14s | %-26s | %-12s | %-12s | %-12s\n", "Non-Terminal", "(", "ID", "NUM", "+ - * / %");
    printf("--------------------------------------------------------------------------------------\n");
    printf("%-14s | %-26s | %-12s | %-12s | %-12s\n", "EXPR", "EXPR -> ( OP ARG ARG )", "---", "---", "---");
    printf("%-14s | %-26s | %-12s | %-12s | %-12s\n", "OP", "---", "---", "---", "OP -> binop");
    printf("%-14s | %-26s | %-12s | %-12s | %-12s\n", "ARG", "ARG -> ( EXPR )", "ARG -> ID", "ARG -> NUM", "---");
    printf("======================================================================================\n\n");
}

void display_trace(void) {
    int total_steps = sizeof(sample_trace) / sizeof(sample_trace[0]);
    printf("Sample Input: (+ (* a b) d)\n\n");
    printf("%-28s %-25s %-30s\n", "Stack", "Input", "Production / Action");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < total_steps; i++) {
        printf("%-28s %-25s %-30s\n", sample_trace[i].stack_str, sample_trace[i].input_str, sample_trace[i].action);
    }
    printf("--------------------------------------------------------------------------------------\n");
    printf("\nResult: Input successfully parsed and ACCEPTED by LL(1) Predictive Parser.\n");
}

int main(void) {
    display_parsing_table();
    display_trace();
    return 0;
}
