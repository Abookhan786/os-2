/*
================================================================================
EX NO: 9  23Z511 – SYSTEM SOFTWARE LABORATORY
SCANNER IMPLEMENTATION: PREDICTIVE PARSING & PARSE TREE ANALYSIS
================================================================================
AIM:
  To implement predictive parsing for arithmetic expressions using an LL(1) 
  grammar, construct and analyze the corresponding parse tree, and use GDB 
  debugging techniques to trace and inspect the execution of the parser program.

OVERVIEW & GRAMMAR:
  Predictive parsing is a top-down parsing technique used for LL(1) grammars. 
  In this experiment, arithmetic expressions are represented in parenthesized 
  list form (Lisp-like prefix notation).

  The grammar used is:
    EXPR -> ( OP ARG ARG )
    OP   -> + | - | * | / | %
    ARG  -> ID | NUM | EXPR

  - EXPR is the start symbol.
  - The grammar supports binary arithmetic operators, identifiers, integers, 
    and nested expressions.
  - Operator nodes form internal nodes; identifiers & numbers form leaf nodes.
  - After parsing, values are assigned to identifiers and the expression is 
    evaluated recursively by traversing the parse tree.

Predictive Parsing Table M(A, a):
  M[EXPR, LP]   = EXPR -> ( OP ARG ARG )
  M[OP, binop]  = OP -> binop
  M[ARG, ID]    = ARG -> ID
  M[ARG, NUM]   = ARG -> NUM
  M[ARG, LP]    = ARG -> ( EXPR )

Sample Input:
  (+ (* a b) d)
  With variable values: a = 2, b = 3, d = 4
  Result = (2 * 3) + 4 = 10
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXPR 1001
#define OP   1002
#define ARG  1003
#define ID   1004
#define NUM  1005
#define LP   1006
#define RP   1007
#define ADD  1008
#define SUB  1009
#define MUL  1010
#define DIV  1011
#define REM  1012
#define INV  1013
#define END_OF_INPUT 0

#define binop(x) ((x == ADD) || (x == SUB) || (x == MUL) || (x == DIV) || (x == REM))

/* Symbol Table Definition */
typedef struct _stnode {
    char *id;
    int value;
    struct _stnode *next;
} stnode;

typedef stnode *symbtbl;

/* Constant Table Definition */
typedef struct _ctnode {
    int value;
    struct _ctnode *next;
} ctnode;

typedef ctnode *constbl;

/* Parsing Stack Definition */
typedef struct _stacknode {
    int item;
    struct _stacknode *next;
} stacknode;

typedef stacknode *stack;

/* Parse Tree Definition */
typedef struct _treenode {
    int type;
    union {
        char *op;
        stnode *id;
        ctnode *num;
    } ref;
    struct _treenode *L;
    struct _treenode *R;
    struct _treenode *P;
} treenode;

typedef treenode *parsetree;

/* Global lexical analyzer state */
static char yytext[256];
static const char *input_cursor = NULL;

static int yylex(void) {
    while (*input_cursor && isspace((unsigned char)*input_cursor)) {
        input_cursor++;
    }

    if (*input_cursor == '\0') {
        yytext[0] = '\0';
        return END_OF_INPUT;
    }

    if (*input_cursor == '(') {
        strcpy(yytext, "(");
        input_cursor++;
        return LP;
    }
    if (*input_cursor == ')') {
        strcpy(yytext, ")");
        input_cursor++;
        return RP;
    }
    if (*input_cursor == '+') {
        strcpy(yytext, "+");
        input_cursor++;
        return ADD;
    }
    if (*input_cursor == '-' && !isdigit((unsigned char)*(input_cursor + 1))) {
        strcpy(yytext, "-");
        input_cursor++;
        return SUB;
    }
    if (*input_cursor == '*') {
        strcpy(yytext, "*");
        input_cursor++;
        return MUL;
    }
    if (*input_cursor == '/') {
        strcpy(yytext, "/");
        input_cursor++;
        return DIV;
    }
    if (*input_cursor == '%') {
        strcpy(yytext, "%");
        input_cursor++;
        return REM;
    }

    if (isdigit((unsigned char)*input_cursor) || (*input_cursor == '-' && isdigit((unsigned char)*(input_cursor + 1)))) {
        int i = 0;
        if (*input_cursor == '-') yytext[i++] = *input_cursor++;
        while (isdigit((unsigned char)*input_cursor)) {
            yytext[i++] = *input_cursor++;
        }
        yytext[i] = '\0';
        return NUM;
    }

    if (isalpha((unsigned char)*input_cursor) || *input_cursor == '_') {
        int i = 0;
        while (isalnum((unsigned char)*input_cursor) || *input_cursor == '_') {
            yytext[i++] = *input_cursor++;
        }
        yytext[i] = '\0';
        return ID;
    }

    yytext[0] = *input_cursor++;
    yytext[1] = '\0';
    return INV;
}

/* Symbol Table Functions */
symbtbl insertid(symbtbl T, char *id_str, stnode **ref) {
    stnode *p = T;
    while (p != NULL) {
        if (!strcmp(p->id, id_str)) {
            *ref = p;
            return T;
        }
        p = p->next;
    }
    p = (stnode *)malloc(sizeof(stnode));
    p->id = (char *)malloc((1 + strlen(id_str)) * sizeof(char));
    strcpy(p->id, id_str);
    p->value = 0;
    p->next = T;
    *ref = p;
    return p;
}

/* Constant Table Functions */
constbl insertnum(constbl C, char *num_str, ctnode **ref) {
    ctnode *p = (ctnode *)malloc(sizeof(ctnode));
    p->value = atoi(num_str);
    p->next = C;
    *ref = p;
    return p;
}

/* Stack Operations */
int top(stack S) {
    if (S == NULL) {
        fprintf(stderr, "*** Read from an empty stack\n");
        exit(1);
    }
    return S->item;
}

stack push(stack S, int item) {
    stacknode *p = (stacknode *)malloc(sizeof(stacknode));
    p->item = item;
    p->next = S;
    return p;
}

stack pop(stack S) {
    if (S == NULL) {
        fprintf(stderr, "*** Pop from an empty stack\n");
        exit(1);
    }
    stacknode *p = S;
    S = S->next;
    free(p);
    return S;
}

/* Predictive Parsing Algorithm */
parsetree parse(symbtbl *T, constbl *C) {
    int A, a;
    stack S = NULL;
    parsetree PT = NULL;
    treenode *p = NULL, *q;

    S = push(S, EXPR);
    a = yylex();

    while (1) {
        if (S == NULL) break;
        A = top(S);
        S = pop(S);

        if (A == EXPR) {
            if (a != LP) {
                fprintf(stderr, "*** Error: LP expected in place of %s\n", yytext);
                exit(2);
            }
            S = push(S, RP);
            S = push(S, ARG);
            S = push(S, ARG);
            S = push(S, OP);
            S = push(S, LP);
        } else if (A == ARG) {
            if (a == ID) {
                S = push(S, ID);
            } else if (a == NUM) {
                S = push(S, NUM);
            } else if (a == LP) {
                S = push(S, EXPR);
            } else {
                fprintf(stderr, "*** Error: ID/NUM/LP expected in place of %s\n", yytext);
                exit(3);
            }
        } else if (A == OP) {
            if (!binop(a)) {
                fprintf(stderr, "*** Error: Invalid operator %s found\n", yytext);
                exit(4);
            }
            S = push(S, a);
        } else {
            // Terminal symbols
            if (A == ID) {
                if (a != ID) {
                    fprintf(stderr, "*** ERROR: ID expected in place of %s\n", yytext);
                    exit(5);
                }
                if ((p == NULL) || ((p->L != NULL) && (p->R != NULL))) {
                    fprintf(stderr, "*** Error: Invalid id %s found\n", yytext);
                    exit(6);
                }
                if (p->L == NULL) q = p->L = (treenode *)malloc(sizeof(treenode));
                else              q = p->R = (treenode *)malloc(sizeof(treenode));

                q->type = ID;
                *T = insertid(*T, yytext, &((q->ref).id));
                q->L = q->R = NULL;
                q->P = p;
            } else if (A == NUM) {
                if (a != NUM) {
                    fprintf(stderr, "*** ERROR: Number expected in place of %s\n", yytext);
                    exit(7);
                }
                if ((p == NULL) || ((p->L != NULL) && (p->R != NULL))) {
                    fprintf(stderr, "*** Error: Invalid number %s found\n", yytext);
                    exit(8);
                }
                if (p->L == NULL) q = p->L = (treenode *)malloc(sizeof(treenode));
                else              q = p->R = (treenode *)malloc(sizeof(treenode));

                q->type = NUM;
                *C = insertnum(*C, yytext, &((q->ref).num));
                q->L = q->R = NULL;
                q->P = p;
            } else if (A == LP) {
                if (a != LP) {
                    fprintf(stderr, "*** Error: Left parenthesis expected in place of %s\n", yytext);
                    exit(9);
                }
            } else if (A == RP) {
                if (a != RP) {
                    fprintf(stderr, "*** Error: Right parenthesis expected in place of %s\n", yytext);
                    exit(10);
                }
                while ((p != PT) && (p->R != NULL)) {
                    p = p->P;
                }
            } else if (binop(A)) {
                if (PT == NULL) {
                    p = PT = (treenode *)malloc(sizeof(treenode));
                    PT->P = NULL;
                } else {
                    if (p->L == NULL)      q = p->L = (treenode *)malloc(sizeof(treenode));
                    else if (p->R == NULL) q = p->R = (treenode *)malloc(sizeof(treenode));
                    else {
                        fprintf(stderr, "*** Error: Cannot create subtree for operator %s\n", yytext);
                        exit(11);
                    }
                    q->P = p;
                    p = q;
                }
                p->type = OP;
                (p->ref).op = (char *)malloc((1 + strlen(yytext)) * sizeof(char));
                strcpy((p->ref).op, yytext);
                p->L = p->R = NULL;
            }

            if (S != NULL) {
                a = yylex();
            }
        }
    }

    if (a != END_OF_INPUT && a != RP) {
        // If remaining tokens exist after empty stack
    }

    printf("Parsing is successful\n");
    return PT;
}

/* Print parse tree */
void prntree(parsetree PT, int level) {
    if (PT == NULL) return;
    for (int l = 0; l < level - 1; ++l) printf("   ");
    if (level) printf(" ---> ");

    if (PT->type == OP) {
        printf("OP(%s)\n", (PT->ref).op);
    } else if (PT->type == ID) {
        printf("ID(%s)\n", (PT->ref).id->id);
    } else if (PT->type == NUM) {
        printf("NUM(%d)\n", (PT->ref).num->value);
    }

    prntree(PT->L, level + 1);
    prntree(PT->R, level + 1);
}

/* Read values for identifiers */
void readvalues(symbtbl T) {
    if (T == NULL) return;
    readvalues(T->next);
    printf("Enter value for %s: ", T->id);
    char buf[64];
    if (fgets(buf, sizeof(buf), stdin)) {
        T->value = atoi(buf);
        printf("%s = %d\n", T->id, T->value);
    }
}

/* Evaluate parse tree */
int evaltree(parsetree PT) {
    if (PT == NULL) return 0;
    switch (PT->type) {
        case ID:
            return ((PT->ref).id)->value;
        case NUM:
            return ((PT->ref).num)->value;
        case OP:
            if (!strcmp((PT->ref).op, "+"))
                return evaltree(PT->L) + evaltree(PT->R);
            if (!strcmp((PT->ref).op, "-"))
                return evaltree(PT->L) - evaltree(PT->R);
            if (!strcmp((PT->ref).op, "*"))
                return evaltree(PT->L) * evaltree(PT->R);
            if (!strcmp((PT->ref).op, "/")) {
                int r = evaltree(PT->R);
                return (r != 0) ? (evaltree(PT->L) / r) : 0;
            }
            if (!strcmp((PT->ref).op, "%")) {
                int r = evaltree(PT->R);
                return (r != 0) ? (evaltree(PT->L) % r) : 0;
            }
    }
    return 0;
}

int main(void) {
    char input[1024];
    const char *default_expr = "(+ (* a b) d)";

    printf("Enter parenthesized prefix expression (or press enter for default: %s):\n", default_expr);
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }
    } else {
        input[0] = '\0';
    }

    const char *expr_str = (strlen(input) > 0) ? input : default_expr;
    input_cursor = expr_str;

    parsetree PT;
    symbtbl T = NULL;
    constbl C = NULL;

    PT = parse(&T, &C);
    printf("\nGenerated Parse Tree:\n");
    prntree(PT, 0);

    if (T) {
        printf("\nReading variable values from the input:\n");
        readvalues(T);
    }

    printf("\nThe expression evaluates to %d\n", evaltree(PT));
    return 0;
}
