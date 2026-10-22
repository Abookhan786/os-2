/*
================================================================================
QUESTION Q2.Y:
Modify the Lex program so that the recognized tokens are passed to Yacc. Write
a Yacc program to parse and validate Boolean expressions.

The expression may contain:
  • identifiers
  • integer constants
  • relational operators: <, >, <=, >=, ==, !=
  • logical operators: &&, ||, !
  • parentheses

The parser must enforce the following precedence, from highest to lowest:
  !
  relational operators
  &&
  ||
All binary operators should be left associative.

Valid examples:
  a < b
  a < b && c > d
  x >= 10 || y == 20
  (a < b || c != d) && x >= 10
  !(a < b) && c > 10
  a < b || c < d && e == f
  The last expression should be interpreted as:
  (a < b) || ((c < d) && (e == f))

Invalid examples:
  a < && b
  x || || y
  (a < b && c
  ! && a
  a < b c > d

Expected output:
  The program should print whether the complete input is a valid Boolean expression:
  "Valid Boolean expression" or "Invalid Boolean expression".

--------------------------------------------------------------------------------
COMPANION YACC & LEX CODE:
--------------------------------------------------------------------------------
--- File: boolean_validator.l ---
%{
#include "y.tab.h"
%}
%%
[ \t]                           { ; }
\n                              { return 0; }
"&&"                            { return AND; }
"||"                            { return OR; }
"!"                             { return NOT; }
"<="                            { return LE; }
">="                            { return GE; }
"=="                            { return EQ; }
"!="                            { return NE; }
"<"                             { return LT; }
">"                             { return GT; }
"("                             { return '('; }
")"                             { return ')'; }
[0-9]+                          { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*          { return ID; }
.                               { return yytext[0]; }
%%
int yywrap() { return 1; }

--- File: boolean_validator.y ---
%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
%token ID NUM AND OR NOT LT GT LE GE EQ NE
%left OR
%left AND
%left LT GT LE GE EQ NE
%right NOT
%%
input: expr { printf("Valid Boolean expression\n"); return 0; }
     ;
expr: expr OR expr
    | expr AND expr
    | expr LT expr
    | expr GT expr
    | expr LE expr
    | expr GE expr
    | expr EQ expr
    | expr NE expr
    | NOT expr
    | '(' expr ')'
    | ID
    | NUM
    ;
%%
void yyerror(const char *s) {
    printf("Invalid Boolean expression\n");
}
int parse_main() {
    printf("Enter Boolean expression:\n");
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
    TOK_INT,
    TOK_RELOP, // <, >, <=, >=, ==, !=
    TOK_AND,   // &&
    TOK_OR,    // ||
    TOK_NOT,   // !
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

static int tokenize(const char *input) {
    token_count = 0;
    cur_token = 0;
    int i = 0;

    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        // Two-character operators
        if (input[i] == '&' && input[i + 1] == '&') {
            tokens[token_count].type = TOK_AND;
            strcpy(tokens[token_count].text, "&&");
            token_count++;
            i += 2;
        } else if (input[i] == '|' && input[i + 1] == '|') {
            tokens[token_count].type = TOK_OR;
            strcpy(tokens[token_count].text, "||");
            token_count++;
            i += 2;
        } else if (input[i] == '<' && input[i + 1] == '=') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, "<=");
            token_count++;
            i += 2;
        } else if (input[i] == '>' && input[i + 1] == '=') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, ">=");
            token_count++;
            i += 2;
        } else if (input[i] == '=' && input[i + 1] == '=') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, "==");
            token_count++;
            i += 2;
        } else if (input[i] == '!' && input[i + 1] == '=') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, "!=");
            token_count++;
            i += 2;
        } else if (input[i] == '!') {
            tokens[token_count].type = TOK_NOT;
            strcpy(tokens[token_count].text, "!");
            token_count++;
            i++;
        } else if (input[i] == '<') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, "<");
            token_count++;
            i++;
        } else if (input[i] == '>') {
            tokens[token_count].type = TOK_RELOP;
            strcpy(tokens[token_count].text, ">");
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
        } else if (isdigit((unsigned char)input[i])) {
            int start = i;
            while (isdigit((unsigned char)input[i])) i++;
            int len = i - start;
            tokens[token_count].type = TOK_INT;
            strncpy(tokens[token_count].text, input + start, len);
            tokens[token_count].text[len] = '\0';
            token_count++;
        } else if (isalpha((unsigned char)input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)input[i]) || input[i] == '_') i++;
            int len = i - start;
            tokens[token_count].type = TOK_ID;
            strncpy(tokens[token_count].text, input + start, len);
            tokens[token_count].text[len] = '\0';
            token_count++;
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
Parser according to precedence:
Highest: !
         Relational operators (<, >, <=, >=, ==, !=)
         &&
Lowest:  ||
*/

static int parse_or();
static int parse_and();
static int parse_rel();
static int parse_not();
static int parse_primary();

static int parse_primary() {
    if (tokens[cur_token].type == TOK_ID || tokens[cur_token].type == TOK_INT) {
        cur_token++;
        return 1;
    }
    if (tokens[cur_token].type == TOK_LPAREN) {
        cur_token++;
        if (!parse_or()) return 0;
        if (tokens[cur_token].type != TOK_RPAREN) return 0;
        cur_token++;
        return 1;
    }
    return 0;
}

static int parse_not() {
    if (tokens[cur_token].type == TOK_NOT) {
        cur_token++;
        return parse_not();
    }
    return parse_primary();
}

static int parse_rel() {
    if (!parse_not()) return 0;
    while (tokens[cur_token].type == TOK_RELOP) {
        cur_token++;
        if (!parse_not()) return 0;
    }
    return 1;
}

static int parse_and() {
    if (!parse_rel()) return 0;
    while (tokens[cur_token].type == TOK_AND) {
        cur_token++;
        if (!parse_rel()) return 0;
    }
    return 1;
}

static int parse_or() {
    if (!parse_and()) return 0;
    while (tokens[cur_token].type == TOK_OR) {
        cur_token++;
        if (!parse_and()) return 0;
    }
    return 1;
}

int validate_boolean_expression(const char *expr) {
    if (!tokenize(expr)) return 0;
    if (tokens[0].type == TOK_END) return 0;
    if (!parse_or()) return 0;
    if (tokens[cur_token].type != TOK_END) return 0;
    return 1;
}

int main(void) {
    char input[256];
    printf("Enter Boolean expression:\n");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }

        if (validate_boolean_expression(input)) {
            printf("Valid Boolean expression\n");
        } else {
            printf("Invalid Boolean expression\n");
        }
    }
    return 0;
}
