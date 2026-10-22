/*
================================================================================
EXPERIMENT 11 - SYNTAX DIRECTED TRANSLATION (SDT)
QUESTION:
Implement a Syntax-Directed Translation (SDT) program to convert an 
arithmetic expression from infix notation to postfix notation.

The program should accept any valid arithmetic expression containing identifiers, 
operators +, -, *, / and parentheses, and generate the corresponding postfix 
expression using semantic actions.

The program should correctly handle operator precedence, associativity, and 
parentheses, and display an appropriate error message for an invalid expression.

Example:
  Input:  a+b*(c-d)
  Output: abcd-*+

Note: The program must work for any valid arithmetic expression entered by the 
user, not only the given example.

--------------------------------------------------------------------------------
COMPANION YACC & LEX SDT IMPLEMENTATION:
--------------------------------------------------------------------------------
--- File: sdt_postfix.l ---
%{
#include "sdt_postfix.tab.h"
#include <string.h>
%}
%%
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }
[0-9]+                  { yylval.str = strdup(yytext); return NUM; }
"+"                     { return '+'; }
"-"                     { return '-'; }
"*"                     { return '*'; }
"/"                     { return '/'; }
"("                     { return '('; }
")"                     { return ')'; }
[ \t]                   { ; }
\n                      { return 0; }
.                       { return yytext[0]; }
%%
int yywrap() { return 1; }

--- File: sdt_postfix.y ---
%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
%union { char *str; }
%token <str> ID NUM
%left '+' '-'
%left '*' '/'
%%
input: expr { printf("\n"); }
     ;
expr: expr '+' expr     { printf("+"); }
    | expr '-' expr     { printf("-"); }
    | expr '*' expr     { printf("*"); }
    | expr '/' expr     { printf("/"); }
    | '(' expr ')'
    | ID                { printf("%s", $1); free($1); }
    | NUM               { printf("%s", $1); free($1); }
    ;
%%
void yyerror(const char *s) {
    fprintf(stderr, "Error: Invalid expression\n");
}
int parse_main() {
    printf("Enter infix expression: ");
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
    char text[64];
} Token;

static Token tokens[1024];
static int token_count = 0;
static int cur_token = 0;

static char postfix_output[4096];
static int postfix_pos = 0;

static void emit(const char *str) {
    int len = (int)strlen(str);
    if (postfix_pos + len < (int)sizeof(postfix_output) - 1) {
        strcpy(postfix_output + postfix_pos, str);
        postfix_pos += len;
    }
}

static int tokenize(const char *input) {
    token_count = 0;
    cur_token = 0;
    postfix_pos = 0;
    postfix_output[0] = '\0';
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
            int start = i;
            while (isdigit((unsigned char)input[i])) i++;
            int len = i - start;
            tokens[token_count].type = TOK_NUM;
            strncpy(tokens[token_count].text, input + start, len);
            tokens[token_count].text[len] = '\0';
            token_count++;
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

/*
Syntax-Directed Translation Grammar with Semantic Actions:
  E -> T E'
  E' -> + T { emit("+") } E'
      | - T { emit("-") } E'
      | ε
  T -> F T'
  T' -> * F { emit("*") } T'
      | / F { emit("/") } T'
      | ε
  F -> ( E )
     | ID  { emit(id) }
     | NUM { emit(num) }
*/

static int parse_E(void);
static int parse_T(void);
static int parse_F(void);

static int parse_F(void) {
    if (tokens[cur_token].type == TOK_ID || tokens[cur_token].type == TOK_NUM) {
        emit(tokens[cur_token].text); // Semantic Action: emit identifier or number
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
        TokenType op = tokens[cur_token].type;
        cur_token++;
        if (!parse_F()) return 0;
        // Semantic Action executed after operand (postfix order):
        if (op == TOK_MUL) emit("*");
        else emit("/");
    }
    return 1;
}

static int parse_E(void) {
    if (!parse_T()) return 0;
    while (tokens[cur_token].type == TOK_PLUS || tokens[cur_token].type == TOK_MINUS) {
        TokenType op = tokens[cur_token].type;
        cur_token++;
        if (!parse_T()) return 0;
        // Semantic Action executed after operand (postfix order):
        if (op == TOK_PLUS) emit("+");
        else emit("-");
    }
    return 1;
}

int translate_infix_to_postfix(const char *input, char *result) {
    if (!tokenize(input)) return 0;
    if (tokens[0].type == TOK_END) return 0;
    if (!parse_E()) return 0;
    if (tokens[cur_token].type != TOK_END) return 0;
    strcpy(result, postfix_output);
    return 1;
}

int main(void) {
    char input[256];
    char result[4096];

    printf("Enter infix expression: ");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }

        if (translate_infix_to_postfix(input, result)) {
            printf("Postfix expression: %s\n", result);
        } else {
            fprintf(stderr, "Error: Invalid arithmetic expression\n");
        }
    }
    return 0;
}
