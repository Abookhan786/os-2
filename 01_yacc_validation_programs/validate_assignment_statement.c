/*
================================================================================
QUESTION Q3.Y:
Modify the Lex program so that the recognized tokens are passed to Yacc. Write
a Yacc program to parse and validate C-like assignment statements.

An assignment statement must have the form:
  identifier = expression ;

The expression may contain:
  • identifiers
  • integer constants
  • floating-point constants
  • +
  • -
  • *
  • /
  • parentheses

The parser must enforce the following precedence:
  Highest: ( )
           * /
           + -
  Lowest
The binary arithmetic operators should be left associative.

Valid examples:
  x = 10;
  total = price * quantity;
  average = (a + b) / 2;
  result = x * 2.5 + y;
  total = price * quantity + tax;
  answer = (a + b) * (c - 2.5);

Invalid examples:
  x = ;
  = 10;
  x = + 10;
  x = a * ;
  x = (a + b;
  x = a b;

Expected output:
  The program should print whether the complete input is a valid assignment statement:
  "Valid assignment statement" or "Invalid assignment statement".

--------------------------------------------------------------------------------
COMPANION YACC & LEX CODE:
--------------------------------------------------------------------------------
--- File: assignment_validator.l ---
%{
#include "y.tab.h"
%}
%%
[ \t]                           { ; }
\n                              { return 0; }
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

--- File: assignment_validator.y ---
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
statement: ID '=' expr ';' { printf("Valid assignment statement\n"); return 0; }
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
    printf("Invalid assignment statement\n");
}
int parse_main() {
    printf("Enter assignment statement:\n");
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

static Token tokens[1024];
static int token_count = 0;
static int cur_token = 0;

static int scan_number(const char *src, int *idx, char *out) {
    int i = *idx;
    int start = i;

    if (!isdigit((unsigned char)src[i])) return 0;
    while (isdigit((unsigned char)src[i])) i++;

    if (src[i] == '.') {
        if (!isdigit((unsigned char)src[i + 1])) return 0; // invalid e.g. 10.
        i++;
        while (isdigit((unsigned char)src[i])) i++;
    }

    if (src[i] == '.') return 0; // extra dot

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

int validate_assignment_statement(const char *stmt) {
    if (!tokenize(stmt)) return 0;
    if (tokens[cur_token].type != TOK_ID) return 0;
    cur_token++;
    if (tokens[cur_token].type != TOK_ASSIGN) return 0;
    cur_token++;
    if (!parse_E()) return 0;
    if (tokens[cur_token].type != TOK_SEMICOLON) return 0;
    cur_token++;
    if (tokens[cur_token].type != TOK_END) return 0;
    return 1;
}

int main(void) {
    char input[256];
    printf("Enter assignment statement:\n");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }

        if (validate_assignment_statement(input)) {
            printf("Valid assignment statement\n");
        } else {
            printf("Invalid assignment statement\n");
        }
    }
    return 0;
}
