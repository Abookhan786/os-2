/*
================================================================================
QUESTION Q4.L:
Write a Lex program to identify and classify tokens in a sequence of C-like 
assignment statements.

The Lex program should recognize:
Identifiers:
  x, total, average, student_count
  Classify them as: IDENTIFIER

Numeric constants:
  • Integer constants: 10, 25, 100       -> INTEGER
  • Floating-point constants: 3.14, 25.5, 0.75 -> FLOAT

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

Whitespace requirement:
  Spaces, tabs and newlines may occur anywhere between tokens and must be 
  ignored by the Lex program.
  For example, the following should produce the same tokens:
    total = price * quantity;
    total =
     price
     *
     quantity
     ;

Sample input:
  total = price * quantity;

  average =
   (a + b) / 2;

  result = x * 2.5 + y;

The Lex program should print the recognized tokens but should not validate 
the syntax of the statements.

--------------------------------------------------------------------------------
COMPANION LEX SPECIFICATION (lex_assignment_sequence.l):
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

void lex_sequence_stream(void) {
    int ch;
    while ((ch = getchar()) != EOF) {
        if (isspace(ch)) {
            continue;
        }

        if (isalpha(ch) || ch == '_') {
            char id_buf[128];
            int len = 0;
            id_buf[len++] = (char)ch;
            while ((ch = getchar()) != EOF && (isalnum(ch) || ch == '_')) {
                if (len < 127) id_buf[len++] = (char)ch;
            }
            id_buf[len] = '\0';
            printf("IDENTIFIER: %s\n", id_buf);
            if (ch != EOF) ungetc(ch, stdin);
        } else if (isdigit(ch)) {
            char num_buf[64];
            int len = 0;
            int is_float = 0;
            num_buf[len++] = (char)ch;
            while ((ch = getchar()) != EOF && isdigit(ch)) {
                if (len < 63) num_buf[len++] = (char)ch;
            }
            if (ch == '.') {
                int next = getchar();
                if (isdigit(next)) {
                    is_float = 1;
                    if (len < 63) num_buf[len++] = '.';
                    if (len < 63) num_buf[len++] = (char)next;
                    while ((ch = getchar()) != EOF && isdigit(ch)) {
                        if (len < 63) num_buf[len++] = (char)ch;
                    }
                    if (ch != EOF) ungetc(ch, stdin);
                } else {
                    if (next != EOF) ungetc(next, stdin);
                    ungetc('.', stdin);
                }
            } else if (ch != EOF) {
                ungetc(ch, stdin);
            }
            num_buf[len] = '\0';
            if (is_float) {
                printf("FLOAT: %s\n", num_buf);
            } else {
                printf("INTEGER: %s\n", num_buf);
            }
        } else if (ch == '=') {
            printf("ASSIGN: =\n");
        } else if (ch == ';') {
            printf("SEMICOLON: ;\n");
        } else if (ch == '+') {
            printf("PLUS: +\n");
        } else if (ch == '-') {
            printf("MINUS: -\n");
        } else if (ch == '*') {
            printf("MULTIPLY: *\n");
        } else if (ch == '/') {
            printf("DIVIDE: /\n");
        } else if (ch == '(') {
            printf("LPAREN: (\n");
        } else if (ch == ')') {
            printf("RPAREN: )\n");
        } else {
            printf("UNKNOWN: %c\n", ch);
        }
    }
}

int main(void) {
    lex_sequence_stream();
    return 0;
}
