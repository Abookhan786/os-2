/*
================================================================================
QUESTION Q2.L:
Write a Lex program to identify and classify tokens in a Boolean expression.

The Lex program should recognize:
Identifiers:
  Examples: a, count, total1, value_2
  Classify them as: IDENTIFIER

Relational operators:
  <, >, <=, >=, ==, !=
  Classify them as: RELATIONAL_OPERATOR

Logical operators:
  &&   -> AND
  ||   -> OR
  !    -> NOT

Parentheses:
  (    -> LPAREN
  )    -> RPAREN

Integer constants:
  Classify as: INTEGER

Whitespace should be ignored.

Sample input:
  (a < b && count >= 10) || !flag

Expected output:
  LPAREN: (
  IDENTIFIER: a
  RELATIONAL_OPERATOR: <
  IDENTIFIER: b
  AND: &&
  IDENTIFIER: count
  RELATIONAL_OPERATOR: >=
  INTEGER: 10
  RPAREN: )
  OR: ||
  NOT: !
  IDENTIFIER: flag

--------------------------------------------------------------------------------
COMPANION LEX SPECIFICATION (lex_boolean_expression.l):
--------------------------------------------------------------------------------
%{
#include <stdio.h>
%}
%%
[ \t\r\n]+                          { ; }
"&&"                                { printf("AND: %s\n", yytext); }
"||"                                { printf("OR: %s\n", yytext); }
"!"                                 { printf("NOT: %s\n", yytext); }
"<="|">="|"=="|"!="|"<"|">"         { printf("RELATIONAL_OPERATOR: %s\n", yytext); }
"("                                 { printf("LPAREN: %s\n", yytext); }
")"                                 { printf("RPAREN: %s\n", yytext); }
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

void lex_boolean_expression(const char *input) {
    int i = 0;

    while (input[i] != '\0') {
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        // Two-character operators
        if (input[i] == '&' && input[i + 1] == '&') {
            printf("AND: &&\n");
            i += 2;
        } else if (input[i] == '|' && input[i + 1] == '|') {
            printf("OR: ||\n");
            i += 2;
        } else if ((input[i] == '<' || input[i] == '>' || input[i] == '=' || input[i] == '!') && input[i + 1] == '=') {
            printf("RELATIONAL_OPERATOR: %c=\n", input[i]);
            i += 2;
        } else if (input[i] == '<' || input[i] == '>') {
            printf("RELATIONAL_OPERATOR: %c\n", input[i]);
            i++;
        } else if (input[i] == '!') {
            printf("NOT: !\n");
            i++;
        } else if (input[i] == '(') {
            printf("LPAREN: (\n");
            i++;
        } else if (input[i] == ')') {
            printf("RPAREN: )\n");
            i++;
        } else if (isdigit((unsigned char)input[i])) {
            int start = i;
            while (isdigit((unsigned char)input[i])) i++;
            int len = i - start;
            char num_buf[64];
            strncpy(num_buf, input + start, len);
            num_buf[len] = '\0';
            printf("INTEGER: %s\n", num_buf);
        } else if (isalpha((unsigned char)input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum((unsigned char)input[i]) || input[i] == '_') i++;
            int len = i - start;
            char id_buf[128];
            strncpy(id_buf, input + start, len);
            id_buf[len] = '\0';
            printf("IDENTIFIER: %s\n", id_buf);
        } else {
            printf("UNKNOWN: %c\n", input[i]);
            i++;
        }
    }
}

int main(void) {
    char input[1024];
    printf("Enter Boolean expression:\n");
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }
        lex_boolean_expression(input);
    }
    return 0;
}
