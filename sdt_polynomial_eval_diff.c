/*
================================================================================
EXPERIMENT 11: SYNTAX DIRECTED TRANSLATION IMPLEMENTATION
================================================================================
AIM:
To study and implement syntax-directed translation using grammar rules and 
semantic actions, understand the translation of input expressions into 
intermediate representations, and verify the output using suitable sample input 
programs.

DESCRIPTION:
This program implements Syntax-Directed Translation for evaluating polynomials,
computing their symbolic derivatives, and displaying the annotated parse tree.
Attributes:
  - 'inh' (Inherited attribute): Propagates sign/multiplier down the parse tree.
  - 'val' (Synthesized attribute): Computes numeric values of coefficients and powers.

Sample Polynomial Input:
  -506x^7 + 9x^10 + x - 243015 - x^3 + 876 x^2 + 1

Outputs Generated:
  1. Annotated Parse Tree with inherited (inh) and synthesized (val) attributes.
  2. Evaluation of polynomial f(x) for x in [-5, 5].
  3. Symbolic Derivative f'(x).

Files in Experiment Record:
  - polyutil.c
  - poly.y
  - poly.l
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PLUS 1
#define MINUS -1

typedef struct _node {
    char symb;
    int nc;
    struct _node **C;
    int inh;
    int val;
} node;

static node *PT = NULL;

/* Node allocation functions matching polyutil.c */
node *addleaf(char c) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = c;
    p->val = 0;
    if ((c >= '0') && (c <= '9')) p->val = c - '0';
    p->nc = 0;
    p->C = NULL;
    p->inh = 0;
    return p;
}

node *addnodeN(node *C1, node *C2) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'N';
    p->val = 0;
    p->inh = 0;
    if (C2 != NULL) {
        p->nc = 2;
        p->C = (node **)malloc(2 * sizeof(node *));
        (p->C)[0] = C1;
        (p->C)[1] = C2;
    } else {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = C1;
    }
    return p;
}

node *addnodeM(node *C1, node *C2) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'M';
    p->val = 0;
    p->inh = 0;
    if (C2 != NULL) {
        p->nc = 2;
        p->C = (node **)malloc(2 * sizeof(node *));
        (p->C)[0] = C1;
        (p->C)[1] = C2;
    } else {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = C1;
    }
    return p;
}

node *addnodeS(char c, node *P) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'S';
    p->val = 0;
    p->inh = 0;
    if (c == '\0') {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = P;
    } else if ((c == '+') || (c == '-')) {
        p->nc = 2;
        p->C = (node **)malloc(2 * sizeof(node *));
        (p->C)[0] = addleaf(c);
        (p->C)[1] = P;
    }
    return p;
}

node *addnodeP(char c, node *C1, node *C2) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'P';
    p->val = 0;
    p->inh = 0;
    if (c == '\0') {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = C1;
    } else if ((c == '+') || (c == '-')) {
        p->nc = 3;
        p->C = (node **)malloc(3 * sizeof(node *));
        (p->C)[0] = C1;
        (p->C)[1] = addleaf(c);
        (p->C)[2] = C2;
    }
    return p;
}

node *addnodeT(char c, node *C1, node *C2) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'T';
    p->val = 0;
    p->inh = 0;
    if ((c == 'N') || (c == 'X')) {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = C1;
    } else if (c == 'B') {
        p->nc = 2;
        p->C = (node **)malloc(2 * sizeof(node *));
        (p->C)[0] = C1;
        (p->C)[1] = C2;
    }
    return p;
}

node *addnodeX(node *C1, node *C2) {
    node *p = (node *)malloc(sizeof(node));
    p->symb = 'X';
    p->val = 0;
    p->inh = 0;
    if (C2 != NULL) {
        p->nc = 3;
        p->C = (node **)malloc(3 * sizeof(node *));
        (p->C)[0] = C1;
        (p->C)[1] = addleaf('^');
        (p->C)[2] = C2;
    } else {
        p->nc = 1;
        p->C = (node **)malloc(sizeof(node *));
        (p->C)[0] = C1;
    }
    return p;
}

/* Syntax Directed Attribute Evaluation */
void setatt(node *p) {
    if (!p) return;
    switch (p->symb) {
    case 'S':
        if (p->nc == 1) {
            (p->C)[0]->inh = PLUS;
            setatt((p->C)[0]);
        } else {
            (p->C)[1]->inh = ((p->C)[0]->symb == '+') ? PLUS : MINUS;
            setatt((p->C)[1]);
        }
        break;
    case 'P':
        (p->C)[0]->inh = p->inh;
        setatt((p->C)[0]);
        if (p->nc == 3) {
            (p->C)[2]->inh = ((p->C)[1]->symb == '+') ? PLUS : MINUS;
            setatt((p->C)[2]);
        }
        break;
    case 'T':
        if (p->nc == 1) {
            if ((p->C)[0]->symb == 'N') (p->C)[0]->inh = 0;
            setatt((p->C)[0]);
        } else {
            setatt((p->C)[0]);
            setatt((p->C)[1]);
        }
        break;
    case 'X':
        if (p->nc == 3) {
            setatt((p->C)[2]);
        }
        break;
    case 'N':
        if (p->nc == 1) {
            p->val = (p->C)[0]->val;
        } else {
            (p->C)[1]->inh = (p->C)[0]->val;
            setatt((p->C)[1]);
            p->val = (p->C)[1]->val;
        }
        break;
    case 'M':
        if (p->nc == 1) {
            p->val = 10 * (p->inh) + (p->C)[0]->val;
        } else {
            (p->C)[1]->inh = 10 * (p->inh) + (p->C)[0]->val;
            setatt((p->C)[1]);
            p->val = (p->C)[1]->val;
        }
        break;
    }
}

void printPT(node *p, int l) {
    int i;
    if (!p) return;
    for (i = 0; i < l - 1; ++i) printf("   ");
    if (l > 1) printf("==> "); else printf(" ");
    printf("%c [", p->symb);
    if ((p->symb == 'P') || (p->symb == 'T'))
        printf("inh = %c", (p->inh == PLUS) ? '+' : '-');
    else if ((p->symb >= '0') && (p->symb <= '9'))
        printf("val = %d", p->val);
    else if (p->symb == 'N')
        printf("val = %d", p->val);
    else if (p->symb == 'M')
        printf("inh = %d, val = %d", p->inh, p->val);
    printf("]\n");
    for (i = 0; i < p->nc; ++i) printPT((p->C)[i], l + 1);
}

int pwr(int a, int e) {
    int t;
    if (e == 0) return 1;
    t = pwr(a, e / 2);
    t = t * t;
    if (e % 2 == 1) t *= a;
    return t;
}

int evalpoly(node *p, int xval) {
    if (!p) return 0;
    if (p->symb == 'S') {
        if (p->nc == 1) return evalpoly((p->C)[0], xval);
        else return evalpoly((p->C)[1], xval);
    }
    if (p->symb == 'P') {
        if (p->nc == 1) return evalpoly((p->C)[0], xval);
        else return evalpoly((p->C)[0], xval) + evalpoly((p->C)[2], xval);
    }
    if (p->symb == 'T') {
        if (p->inh == PLUS) {
            if (p->nc == 1) return evalpoly((p->C)[0], xval);
            else return evalpoly((p->C)[0], xval) * evalpoly((p->C)[1], xval);
        } else {
            if (p->nc == 1) return -(evalpoly((p->C)[0], xval));
            else return -(evalpoly((p->C)[0], xval)) * evalpoly((p->C)[1], xval);
        }
    }
    if (p->symb == 'X') {
        if (p->nc == 1) return xval;
        else return pwr(xval, evalpoly((p->C)[2], xval));
    }
    if ((p->symb == 'N') || (p->symb == 'M')) return p->val;
    if (p->symb >= '1') return 1;
    return 0;
}

void prnterm(int C, int E) {
    if (C == 0) return;
    if (E == 0) {
        printf("%d", C);
    } else if (E == 1) {
        if (C != 1) printf("%d", C);
        printf("x");
    } else {
        if (C != 1) printf("%d", C);
        printf("x^%d", E);
    }
}

void printderivative(node *p) {
    node *q;
    if (!p) return;
    if (p->symb == 'S') {
        if (p->nc == 1) printderivative((p->C)[0]);
        else printderivative((p->C)[1]);
    } else if (p->symb == 'P') {
        if (p->nc == 1) printderivative((p->C)[0]);
        else {
            printderivative((p->C)[0]);
            printderivative((p->C)[2]);
        }
    } else if (p->symb == 'T') {
        if (p->nc == 1) {
            if ((p->C)[0]->symb == 'X') {
                if (p->inh == PLUS) printf(" + "); else printf(" - ");
                if (((p->C)[0])->nc == 1) prnterm(1, 0);
                else prnterm(((p->C)[0]->C)[2]->val, ((p->C)[0]->C)[2]->val - 1);
            }
        } else {
            if (p->inh == PLUS) printf(" + "); else printf(" - ");
            q = (p->C)[1];
            if (q->nc == 1) prnterm((p->C)[0]->val, 0);
            else prnterm(((p->C)[0]->val) * ((q->C)[2]->val), (q->C)[2]->val - 1);
        }
    }
}

/* Lexer and Parser for polynomial expression string */
static const char *cursor;

static void skip_ws() {
    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n') cursor++;
}

static node *parse_N_num(int val) {
    char s[32];
    sprintf(s, "%d", val);
    int len = (int)strlen(s);
    if (len == 1) {
        return addnodeN(addleaf(s[0]), NULL);
    }
    // Build M chain
    node *m = addleaf(s[len - 1]);
    for (int i = len - 2; i >= 1; i--) {
        m = addnodeM(addleaf(s[i]), m);
    }
    return addnodeN(addleaf(s[0]), m);
}

static node *parse_polynomial_string(const char *str) {
    cursor = str;
    skip_ws();
    char lead_sign = '\0';
    if (*cursor == '+' || *cursor == '-') {
        lead_sign = *cursor;
        cursor++;
        skip_ws();
    }

    node *head_p = NULL;

    while (*cursor != '\0') {
        skip_ws();
        if (*cursor == '\0') break;

        char op_sign = '+';
        if (head_p != NULL) {
            if (*cursor == '+' || *cursor == '-') {
                op_sign = *cursor;
                cursor++;
                skip_ws();
            }
        }

        // Parse Term T: could be [N] [x [^ N]]
        int has_coef = 0;
        int coef = 1;
        if (isdigit((unsigned char)*cursor)) {
            has_coef = 1;
            coef = 0;
            while (isdigit((unsigned char)*cursor)) {
                coef = coef * 10 + (*cursor - '0');
                cursor++;
            }
            skip_ws();
        }

        node *node_t = NULL;
        if (*cursor == 'x') {
            cursor++;
            skip_ws();
            node *node_x = NULL;
            if (*cursor == '^') {
                cursor++;
                skip_ws();
                int exp_val = 0;
                while (isdigit((unsigned char)*cursor)) {
                    exp_val = exp_val * 10 + (*cursor - '0');
                    cursor++;
                }
                node_x = addnodeX(addleaf('x'), parse_N_num(exp_val));
            } else {
                node_x = addnodeX(addleaf('x'), NULL);
            }

            if (has_coef) {
                node_t = addnodeT('B', parse_N_num(coef), node_x);
            } else {
                node_t = addnodeT('X', node_x, NULL);
            }
        } else if (has_coef) {
            node_t = addnodeT('N', parse_N_num(coef), NULL);
        }

        if (!node_t) break;

        if (head_p == NULL) {
            head_p = addnodeP('\0', node_t, NULL);
        } else {
            head_p = addnodeP(op_sign, head_p, node_t);
        }
        skip_ws();
    }

    return addnodeS(lead_sign, head_p);
}

int main(int argc, char *argv[]) {
    char input[1024];
    const char *default_poly = "-506x^7 + 9x^10 + x - 243015 - x^3 + 876 x^2 + 1";

    printf("Enter polynomial (or press enter for default: %s):\n", default_poly);
    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r')) {
            input[--len] = '\0';
        }
    } else {
        input[0] = '\0';
    }

    const char *poly_str = (strlen(input) > 0) ? input : default_poly;
    printf("\nParsing polynomial: %s\n", poly_str);

    PT = parse_polynomial_string(poly_str);
    setatt(PT);

    printf("+++ The annotated parse tree is\n");
    printPT(PT, 1);
    printf("\n");

    for (int xval = -5; xval <= 5; ++xval) {
        printf("+++ f(%2d) = %10d\n", xval, evalpoly(PT, xval));
    }

    printf("\n+++ f'(x) = ");
    printderivative(PT);
    printf("\n");

    return 0;
}
