/*
================================================================================
QUESTION Q1.Y:
Modify the Lex program so that it passes the recognized tokens to Yacc. Write a
Yacc program to parse and validate arithmetic expressions containing integer,
floating-point, and scientific-notation constants.

The parser should support:
Numeric constants:
  12, 3.14, 2.5e10, 3E-4
Operators:
  +, -, *, /
Parentheses:
  (, )

Grammar precedence & associativity:
  Highest: ( )
           * /
           + -
  Lowest
  The binary arithmetic operators should be left associative.

Valid expressions:
  12 + 3.14
  12 + 3.14 * 5
  (12 + 3.14) * 5
  2.5e10 / (10 + 5)
  12 + 3.5 * 2.0e-3
  (2 + 3) * (4 - 1.5)

Invalid expressions:
  12 + * 5
  (12 + 5
  3.14 5
  12 / / 4
  + 12 * 5

Expected behaviour:
  For a valid expression:
    Enter expression:
    12 + 3.14 * 5
    Valid arithmetic expression
  For an invalid expression:
    Enter expression:
    12 + * 5
    Invalid arithmetic expression

--------------------------------------------------------------------------------
COMPANION YACC & LEX CODE:
--------------------------------------------------------------------------------
--- File: arithmetic_validator.l ---
%{
#include "y.tab.h"
%}
%%
[0-9]+                          { return NUM; }
[0-9]+\.[0-9]+                  { return NUM; }
[0-9]+(\.[0-9]+)?[eE][+-]?[0-9]+ { return NUM; }
[ \t]                           { ; }
\n                              { return 0; }
"+"                             { return '+'; }
"-"                             { return '-'; }
"*"                             { return '*'; }
"/"                             { return '/'; }
"("                             { return '('; }
")"                             { return ')'; }
.                               { return yytext[0]; }
%%
int yywrap() { return 1; }

--- File: arithmetic_validator.y ---
%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
%token NUM
%left '+' '-'
%left '*' '/'
%%
input: expr { printf("Valid arithmetic expression\n"); return 0; }
     ;
expr: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | NUM
    ;
%%
void yyerror(const char *s) {
    printf("Invalid arithmetic expression\n");
}
int parse_main() {
    printf("Enter expression:\n");
    yyparse();
    return 0;
}
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOK_NUM,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_END,
    TOK_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char text[128];
} Token;

static Token tokens[1024];
static int token_count = 0;
static int cur_token = 0;

/* Check if string is a valid scientific or float or int constant */
static int scan_number(const char *src, int *idx, char *out) {
    int i = *idx;
    int start = i;
    int has_dot = 0;
    int has_exp = 0;

    // Digits before dot
    if (!isdigit((unsigned char)src[i])) return 0;
    while (isdigit((unsigned char)src[i])) i++;

    // Optional fractional part
    if (src[i] == '.') {
        // Lookahead: must have digit after dot
        if (!isdigit((unsigned char)src[i + 1])) {
            return 0; // Invalid like 12.
        }
        has_dot = 1;
        i++;
        while (isdigit((unsigned char)src[i])) i++;
    }

    // Optional exponent part
    if (src[i] == 'e' || src[i] == 'E') {
        int e_idx = i;
        i++;
        if (src[i] == '+' || src[i] == '-') i++;
        if (!isdigit((unsigned char)src[i])) {
            return 0; // Invalid like 2e
        }
        while (isdigit((unsigned char)src[i])) i++;
        has_exp = 1;
    }

    // Check if next char is dot (e.g. 3.4.5)
    if (src[i] == '.') return 0;

    int len = i - start;
    strncpy(out, src + start, len);
    out[len] = '\0';
    *idx = i;
    return 1;
}

static int tokenize(const char *input) {
    token_count = 0;
    cur_token = 0;
    int i = 0;

    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        if (isdigit((unsigned char)input[i])) {
            char num_buf[128];
            if (scan_number(input, &i, num_buf)) {
                tokens[token_count].type = TOK_NUM;
                strcpy(tokens[token_count].text, num_buf);
                token_count++;
            } else {
                tokens[token_count].type = TOK_INVALID;
                token_count++;
                return 0;
            }
        } else if (input[i] == '+') {
            tokens[token_count].type = TOK_PLUS;
            strcpy(tokens[token_count].text, "+");
            token_count++;
            i++;
        } else if (input[i] == '-') {
            tokens[token_count].type = TOK_MINUS;
            strcpy(tokens[token_count].text, "-");
            token_count++;
            i++;
        } else if (input[i] == '*') {
            tokens[token_count].type = TOK_MUL;
            strcpy(tokens[token_count].text, "*");
            token_count++;
            i++;
        } else if (input[i] == '/') {
            tokens[token_count].type = TOK_DIV;
            strcpy(tokens[token_count].text, "/");
            token_count++;
            i++;
        } else if (input[i] == '(') {
            tokens[token_count].type = TOK_LPAREN;
            strcpy(tokens[token_count].text, "(");
            token_count++;
            i++;
        } else if (input[i] == ')') {
            tokens[token_count].type = TOK_RPAREN;
            strcpy(tokens[token_count].text, ")");
            token_count++;
            i++;
        } else {
            tokens[token_count].type = TOK_INVALID;
            token_count++;
            return 0;
        }
    }

    tokens[token_count].type = TOK_END;
    tokens[token_count].text[0] = '\0';
    token_count++;
    return 1;
}

/* Recursive Descent Parser adhering to Yacc precedence */
static int parse_E();
static int parse_T();
static int parse_F();

static int parse_F() {
    if (tokens[cur_token].type == TOK_NUM) {
        cur_token++;
        return 1;
    } else if (tokens[cur_token].type == TOK_LPAREN) {
        cur_token++;
        if (!parse_E()) return 0;
        if (tokens[cur_token].type == TOK_RPAREN) {
            cur_token++;
            return 1;
        }
        return 0;
    }
    return 0;
}

static int parse_T() {
    if (!parse_F()) return 0;
    while (tokens[cur_token].type == TOK_MUL || tokens[cur_token].type == TOK_DIV) {
        cur_token++;
        if (!parse_F()) return 0;
    }
    return 1;
}

static int parse_E() {
    if (!parse_T()) return 0;
    while (tokens[cur_token].type == TOK_PLUS || tokens[cur_token].type == TOK_MINUS) {
        cur_token++;
        if (!parse_T()) return 0;
    }
    return 1;
}

int validate_arithmetic_expression(const char *expr) {
    if (!tokenize(expr)) return 0;
    if (tokens[0].type == TOK_END) return 0; // empty expression
    if (!parse_E()) return 0;
    if (tokens[cur_token].type != TOK_END) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    char input[256];

    printf("Enter expression:\n");
    if (fgets(input, sizeof(input), stdin)) {
        // remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[len - 1] = '\0';
        }
        if (len > 1 && (input[len - 2] == '\r')) {
            input[len - 2] = '\0';
        }

        if (validate_arithmetic_expression(input)) {
            printf("Valid arithmetic expression\n");
        } else {
            printf("Invalid arithmetic expression\n");
        }
    }
    return 0;
}
