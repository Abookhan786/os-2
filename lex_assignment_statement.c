/*
================================================================================
QUESTION Q3.L:
Write a Lex program to identify and classify tokens in C-like assignment statements.

The Lex program should recognize:
Identifiers:
  x, total, average, student_count
  Classify them as: IDENTIFIER

Numeric constants:
  Integer constants: 10, 25, 100        -> INTEGER
  Floating-point constants: 3.14, 25.5, 0.75 -> FLOAT

Operators:
  =   -> ASSIGN
  +   -> PLUS
  -   -> MINUS
  *   -> MULTIPLY
  /   -> DIVIDE

Parentheses and statement terminator:
  (   -> LPAREN
  )   -> RPAREN
  ;   -> SEMICOLON

Whitespace should be ignored.

Sample input 1:
  total = price * quantity + 10;

Expected output 1:
  IDENTIFIER: total
  ASSIGN: =
  IDENTIFIER: price
  MULTIPLY: *
  IDENTIFIER: quantity
  PLUS: +
  INTEGER: 10
  SEMICOLON: ;

Sample input 2:
  average = (a + b + 3.14) / 2;

Expected output 2:
  IDENTIFIER: average
  ASSIGN: =
  LPAREN: (
  IDENTIFIER: a
  PLUS: +
  IDENTIFIER: b
  PLUS: +
  FLOAT: 3.14
  RPAREN: )
  DIVIDE: /
  INTEGER: 2
  SEMICOLON: ;

--------------------------------------------------------------------------------
COMPANION LEX SPECIFICATION (lex_assignment_statement.l):
--------------------------------------------------------------------------------
%{
#include <stdio.h>
%}
%%
[ \t\r\n]+                          { ; }
";"                                 { printf("SEMICOLON: ;\n"); }
"="                                 { printf("ASSIGN: =\n"); }
"+"                                 { printf("PLUS: +\n"); }
"-"                                 { printf("MINUS: -\n"); }
"*"                                 { printf("MULTIPLY: *\n"); }
"/"                                 { printf("DIVIDE: /\n"); }
"("                                 { printf("LPAREN: (\n"); }
")"                                 { printf("RPAREN: )\n"); }
[0-9]+\.[0-9]+                      { printf("FLOAT: %s\n", yytext); }
[0-9]+                              { printf("INTEGER: %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]*              { printf("IDENTIFIER: %s\n", yytext); }
.                                   { printf("UNKNOWN: %s\n", yytext); }
%%
int yywrap() { return 1; }
int main() {
    yylex();
    return 0;
}
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void lex_assignment_statement(const char *input) {
    int i = 0;

    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        if (isalpha((unsigned char)input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)input[i]) || input[i] == '_') i++;
            int len = i - start;
            char id_buf[128];
            strncpy(id_buf, input + start, len);
            id_buf[len] = '\0';
            printf("IDENTIFIER: %s\n", id_buf);
        } else if (isdigit((unsigned char)input[i])) {
            int start = i;
            int is_float = 0;
            while (isdigit((unsigned char)input[i])) i++;
            if (input[i] == '.' && isdigit((unsigned char)input[i + 1])) {
                is_float = 1;
                i++;
                while (isdigit((unsigned char)input[i])) i++;
            }
            int len = i - start;
            char num_buf[64];
            strncpy(num_buf, input + start, len);
            num_buf[len] = '\0';
            if (is_float) {
                printf("FLOAT: %s\n", num_buf);
            } else {
                printf("INTEGER: %s\n", num_buf);
            }
        } else if (input[i] == '=') {
            printf("ASSIGN: =\n");
            i++;
        } else if (input[i] == ';') {
            printf("SEMICOLON: ;\n");
            i++;
        } else if (input[i] == '+') {
            printf("PLUS: +\n");
            i++;
        } else if (input[i] == '-') {
            printf("MINUS: -\n");
            i++;
        } else if (input[i] == '*') {
            printf("MULTIPLY: *\n");
            i++;
        } else if (input[i] == '/') {
            printf("DIVIDE: /\n");
            i++;
        } else if (input[i] == '(') {
            printf("LPAREN: (\n");
            i++;
        } else if (input[i] == ')') {
            printf("RPAREN: )\n");
            i++;
        } else {
            printf("UNKNOWN: %c\n", input[i]);
            i++;
        }
    }
}

int main(void) {
    char input[1024];
    printf("Enter assignment statement:\n");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }
        lex_assignment_statement(input);
    }
    return 0;
}
