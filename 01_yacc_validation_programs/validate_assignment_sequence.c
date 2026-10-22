/*
================================================================================
QUESTION Q4.Y:
Modify the Lex program so that the recognized tokens are passed to Yacc. Write
a Yacc program to parse and validate a sequence of C-like assignment statements.

Each statement must have the form:
  identifier = expression ;

An expression may contain:
  • identifiers
  • integer constants
  • floating-point constants
  • +
  • -
  • *
  • /
  • parentheses

The input may contain arbitrary spaces, tabs and newlines between tokens.
The parser should support multiple assignment statements in the same input.
The arithmetic precedence should be:
  Highest: ( )
           * /
           + -
  Lowest
The binary arithmetic operators should be left associative.

Valid examples:
  x = 10;
  total = price * quantity;
  total = price * quantity + tax;
  average = (a + b) / 2;
  result = x * 2.5 + y;
  final_value =
   (a + b) * (c - 10);

The following input should also be valid:
  total
  =
  price
  *
  quantity
  ;
  average
  =
  (
  a
  +
  b
  )
  /
  2
  ;

Invalid examples:
  x = ;
  = 10;
  x = a * ;
  x = (a + b;
  x = a b;
  x = 10
  y = 20;

Expected output:
  The program should print whether the complete input consists of valid assignment statements:
  "Valid sequence of assignment statements" or "Invalid sequence of assignment statements".

--------------------------------------------------------------------------------
COMPANION YACC & LEX CODE:
--------------------------------------------------------------------------------
--- File: assignment_sequence_validator.l ---
%{
#include "y.tab.h"
%}
%%
[ \t\r\n]+                      { ; }
";"                             { return ';'; }
"="                             { return '='; }
"+"                             { return '+'; }
"-"                             { return '-'; }
"*"                             { return '*'; }
"/"                             { return '/'; }
"("                             { return '('; }
")"                             { return ')'; }
[0-9]+\.[0-9]+                  { return NUM; }
[0-9]+                          { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*          { return ID; }
.                               { return yytext[0]; }
%%
int yywrap() { return 1; }

--- File: assignment_sequence_validator.y ---
%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
%token ID NUM
%left '+' '-'
%left '*' '/'
%%
program: stmt_list { printf("Valid sequence of assignment statements\n"); return 0; }
       ;
stmt_list: stmt_list stmt
         | stmt
         ;
stmt: ID '=' expr ';'
    ;
expr: expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | ID
    | NUM
    ;
%%
void yyerror(const char *s) {
    printf("Invalid sequence of assignment statements\n");
}
int parse_main() {
    printf("Enter statements (Ctrl+Z or Ctrl+D to finish):\n");
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
    TOK_ID,
    TOK_NUM,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_SEMICOLON,
    TOK_END,
    TOK_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
} Token;

static Token tokens[4096];
static int token_count = 0;
static int cur_token = 0;

static int scan_number(const char *src, int *idx, char *out) {
    int i = *idx;
    int start = i;

    if (!isdigit((unsigned char)src[i])) return 0;
    while (isdigit((unsigned char)src[i])) i++;

    if (src[i] == '.') {
        if (!isdigit((unsigned char)src[i + 1])) return 0;
        i++;
        while (isdigit((unsigned char)src[i])) i++;
    }

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

        if (isalpha((unsigned char)input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)input[i]) || input[i] == '_') i++;
            int len = i - start;
            tokens[token_count].type = TOK_ID;
            strncpy(tokens[token_count].text, input + start, len);
            tokens[token_count].text[len] = '\0';
            token_count++;
        } else if (isdigit((unsigned char)input[i])) {
            char num_buf[64];
            if (scan_number(input, &i, num_buf)) {
                tokens[token_count].type = TOK_NUM;
                strcpy(tokens[token_count].text, num_buf);
                token_count++;
            } else {
                tokens[token_count].type = TOK_INVALID;
                token_count++;
                return 0;
            }
        } else if (input[i] == '=') {
            tokens[token_count].type = TOK_ASSIGN;
            strcpy(tokens[token_count].text, "=");
            token_count++;
            i++;
        } else if (input[i] == ';') {
            tokens[token_count].type = TOK_SEMICOLON;
            strcpy(tokens[token_count].text, ";");
            token_count++;
            i++;
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

static int parse_E(void);
static int parse_T(void);
static int parse_F(void);
static int parse_statement(void);
static int parse_statement_list(void);

static int parse_F(void) {
    if (tokens[cur_token].type == TOK_ID || tokens[cur_token].type == TOK_NUM) {
        cur_token++;
        return 1;
    }
    if (tokens[cur_token].type == TOK_LPAREN) {
        cur_token++;
        if (!parse_E()) return 0;
        if (tokens[cur_token].type != TOK_RPAREN) return 0;
        cur_token++;
        return 1;
    }
    return 0;
}

static int parse_T(void) {
    if (!parse_F()) return 0;
    while (tokens[cur_token].type == TOK_MUL || tokens[cur_token].type == TOK_DIV) {
        cur_token++;
        if (!parse_F()) return 0;
    }
    return 1;
}

static int parse_E(void) {
    if (!parse_T()) return 0;
    while (tokens[cur_token].type == TOK_PLUS || tokens[cur_token].type == TOK_MINUS) {
        cur_token++;
        if (!parse_T()) return 0;
    }
    return 1;
}

static int parse_statement(void) {
    if (tokens[cur_token].type != TOK_ID) return 0;
    cur_token++;
    if (tokens[cur_token].type != TOK_ASSIGN) return 0;
    cur_token++;
    if (!parse_E()) return 0;
    if (tokens[cur_token].type != TOK_SEMICOLON) return 0;
    cur_token++;
    return 1;
}

static int parse_statement_list(void) {
    if (!parse_statement()) return 0;
    while (tokens[cur_token].type != TOK_END) {
        if (!parse_statement()) return 0;
    }
    return 1;
}

int validate_assignment_sequence(const char *input) {
    if (!tokenize(input)) return 0;
    if (tokens[0].type == TOK_END) return 0; // Empty input is not a valid statement list
    if (!parse_statement_list()) return 0;
    if (tokens[cur_token].type != TOK_END) return 0;
    return 1;
}

int main(void) {
    char buffer[16384];
    size_t total = 0;
    char line[512];

    printf("Enter sequence of assignment statements (Press EOF or blank line on terminal to end):\n");
    buffer[0] = '\0';

    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len + total < sizeof(buffer) - 1) {
            strcat(buffer, line);
            total += len;
        }
    }

    if (validate_assignment_sequence(buffer)) {
        printf("Valid sequence of assignment statements\n");
    } else {
        printf("Invalid sequence of assignment statements\n");
    }

    return 0;
}
