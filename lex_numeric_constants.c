/*
================================================================================
QUESTION Q1.L:
Write a Lex program to identify and classify numeric constants and arithmetic 
symbols from the given input.

The Lex program should recognize and print the following token types:
  Input          Token
  12             INTEGER
  3.14           FLOAT
  2.5e10         SCIENTIFIC
  3E-4           SCIENTIFIC
  +              PLUS
  -              MINUS
  *              MULTIPLY
  /              DIVIDE
  (              LPAREN
  )              RPAREN

The following should not be classified as valid numeric constants:
  12.
  .25
  2e
  3.4.5

Sample input:
  12 + 3.14 * (2.5e10 - 3E-4)

Expected output:
  INTEGER: 12
  PLUS: +
  FLOAT: 3.14
  MULTIPLY: *
  LPAREN: (
  SCIENTIFIC: 2.5e10
  MINUS: -
  SCIENTIFIC: 3E-4
  RPAREN: )

--------------------------------------------------------------------------------
COMPANION LEX SPECIFICATION (lex_numeric_constants.l):
--------------------------------------------------------------------------------
%{
#include <stdio.h>
%}
DIGIT       [0-9]
INT         {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
SCIENTIFIC  {DIGIT}+(\.{DIGIT}+)?[eE][+-]?{DIGIT}+
%%
{SCIENTIFIC} { printf("SCIENTIFIC: %s\n", yytext); }
{FLOAT}      { printf("FLOAT: %s\n", yytext); }
{INT}        { printf("INTEGER: %s\n", yytext); }
"+"          { printf("PLUS: %s\n", yytext); }
"-"          { printf("MINUS: %s\n", yytext); }
"*"          { printf("MULTIPLY: %s\n", yytext); }
"/"          { printf("DIVIDE: %s\n", yytext); }
"("          { printf("LPAREN: %s\n", yytext); }
")"          { printf("RPAREN: %s\n", yytext); }
[ \t\r\n]+   { ; }
.            { printf("INVALID: %s\n", yytext); }
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

/*
Returns:
  1: Scientific
  2: Float
  3: Integer
  0: Invalid numeric constant (e.g. 12., .25, 2e, 3.4.5)
*/
static int classify_number(const char *src, int *idx, char *token_str, int *out_type) {
    int i = *idx;
    int start = i;

    // Must start with a digit (rejects .25)
    if (!isdigit((unsigned char)src[i])) return 0;
    while (isdigit((unsigned char)src[i])) i++;

    int is_float = 0;
    int is_sci = 0;

    // Optional fractional part
    if (src[i] == '.') {
        // Lookahead: digit must follow '.'
        if (!isdigit((unsigned char)src[i + 1])) {
            // e.g. 12. - invalid constant
            int len = (i + 1) - start;
            strncpy(token_str, src + start, len);
            token_str[len] = '\0';
            *idx = i + 1;
            return 0;
        }
        is_float = 1;
        i++;
        while (isdigit((unsigned char)src[i])) i++;
    }

    // Optional exponent part
    if (src[i] == 'e' || src[i] == 'E') {
        int e_idx = i;
        int next_idx = i + 1;
        if (src[next_idx] == '+' || src[next_idx] == '-') next_idx++;
        if (!isdigit((unsigned char)src[next_idx])) {
            // e.g. 2e - invalid constant
            int len = (e_idx + 1) - start;
            strncpy(token_str, src + start, len);
            token_str[len] = '\0';
            *idx = e_idx + 1;
            return 0;
        }
        is_sci = 1;
        i = next_idx;
        while (isdigit((unsigned char)src[i])) i++;
    }

    // Check if there is another '.' immediately following (e.g. 3.4.5)
    if (src[i] == '.') {
        // Malformed numeric string like 3.4.5
        while (isdigit((unsigned char)src[i]) || src[i] == '.') i++;
        int len = i - start;
        strncpy(token_str, src + start, len);
        token_str[len] = '\0';
        *idx = i;
        return 0;
    }

    int len = i - start;
    strncpy(token_str, src + start, len);
    token_str[len] = '\0';
    *idx = i;

    if (is_sci) {
        *out_type = 1;
    } else if (is_float) {
        *out_type = 2;
    } else {
        *out_type = 3;
    }
    return 1;
}

void lex_numeric_and_arithmetic(const char *input) {
    int i = 0;
    char token_buf[128];

    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        if (isdigit((unsigned char)input[i])) {
            int num_type = 0;
            if (classify_number(input, &i, token_buf, &num_type)) {
                if (num_type == 1) {
                    printf("SCIENTIFIC: %s\n", token_buf);
                } else if (num_type == 2) {
                    printf("FLOAT: %s\n", token_buf);
                } else if (num_type == 3) {
                    printf("INTEGER: %s\n", token_buf);
                }
            } else {
                printf("INVALID NUMERIC CONSTANT: %s\n", token_buf);
            }
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
            printf("INVALID: %c\n", input[i]);
            i++;
        }
    }
}

int main(void) {
    char input[1024];
    printf("Enter expression:\n");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }
        lex_numeric_and_arithmetic(input);
    }
    return 0;
}
