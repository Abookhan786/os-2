/*
================================================================================
EXPERIMENT 10: EXPRESSION EVALUATION USING AST & SYMBOL TABLE
================================================================================
QUESTIONS:
Implement an expression evaluation program using Lex and Yacc to evaluate 
arithmetic expressions containing identifiers, integer constants, and operators 
such as addition (+), subtraction (-), multiplication (*), division (/), 
modulus (%), and exponentiation (^).

The program should maintain a symbol table to store identifiers and their values, 
construct an expression tree using leaf nodes and operator nodes, and evaluate the 
expression tree to obtain the final result. It should handle operator precedence, 
parentheses, and variable assignments, while displaying appropriate error 
messages for invalid identifiers, uninitialized variables, and invalid expressions.

Sample Session (from Lab Record Ex 10 Output):
  Input:  (set a 5)
  Output: Variable a is set to 5

  Input:  (set b 9)
  Output: Variable b is set to 9

  Input:  (set c (+ a b))
  Output: Variable c is set to 14

  Input:  (* a b)
  Output: Standalone expression evaluates to 45

  Input:  (set a
  Output: Error: syntax error

--------------------------------------------------------------------------------
COMPANION LEX & YACC FILES INCLUDED IN EXPERIMENT RECORD:
  - runprog.c
  - expr.y
  - expr.l
================================================================================
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM 1
#define ID  2
#define OP  3

#define ADD 101
#define SUB 102
#define MUL 103
#define DIV 104
#define REM 105
#define POW 106

/* Symbol Table Definition */
typedef struct {
    int type;        /* NUM or ID */
    char *data;      /* Name of identifier or string of number */
    int value;       /* Numeric value */
    int init;        /* Initialized (1) or not (0) */
} stentry;

static stentry ST[1024];
static int nsymb = 0;

/* Expression Tree Definition */
typedef struct _node {
    int type;        /* OP, ID, or NUM */
    int data;        /* Opcode or index in symbol table */
    struct _node *L, *R;
} node;

int addsymbol(int type, const char *text) {
    int i;
    for (i = 0; i < nsymb; ++i) {
        if (!strcmp(ST[i].data, text)) return i;
    }
    i = nsymb;
    ++nsymb;
    ST[i].type = type;
    ST[i].data = (char *)malloc((1 + strlen(text)) * sizeof(char));
    strcpy(ST[i].data, text);
    if (type == ID) {
        ST[i].value = 0;
        ST[i].init = 0;
    } else if (type == NUM) {
        ST[i].value = atoi(text);
        ST[i].init = 1;
    } else {
        fprintf(stderr, "Error: Unknown type %d for symbol table\n", type);
        --nsymb;
        return -1;
    }
    return i;
}

int setsymbol(int i, int val) {
    if ((i < 0) || (i >= nsymb)) {
        fprintf(stderr, "Error: Invalid index %d in symbol table\n", i);
        return -1;
    }
    if (ST[i].type != ID) {
        fprintf(stderr, "Error: Attempt to set non-ID in symbol table\n");
        return -1;
    }
    ST[i].value = val;
    ST[i].init = 1;
    return i;
}

int getsymbol(const char *id, int *err) {
    int i;
    for (i = 0; i < nsymb; ++i) {
        if (!strcmp(ST[i].data, id)) {
            if (ST[i].init == 0) {
                fprintf(stderr, "Error: Attempt to read uninitialized value\n");
                if (err) *err = 1;
                return 0;
            }
            return ST[i].value;
        }
    }
    fprintf(stderr, "Error: Invalid ID %s\n", id);
    if (err) *err = 1;
    return 0;
}

int getvalue(int i, int *err) {
    if ((i < 0) || (i >= nsymb)) {
        fprintf(stderr, "Error: Unknown index %d in symbol table\n", i);
        if (err) *err = 1;
        return 0;
    }
    if (ST[i].init == 0) {
        fprintf(stderr, "Error: Attempt to read uninitialized value\n");
        if (err) *err = 1;
        return 0;
    }
    return ST[i].value;
}

node *addleaf(int i) {
    if ((i < 0) || (i >= nsymb)) {
        fprintf(stderr, "Invalid index %d in symbol table\n", i);
        return NULL;
    }
    node *p = (node *)malloc(sizeof(node));
    p->type = ST[i].type;
    p->data = i;
    p->L = p->R = NULL;
    return p;
}

node *addnode(int op, node *left, node *right) {
    node *p = (node *)malloc(sizeof(node));
    p->type = OP;
    p->data = op;
    p->L = left;
    p->R = right;
    return p;
}

int intpow(int a, int e) {
    int prod = 1, a2pow = a;
    if (e < 0) return 0;
    while (e > 0) {
        if (e % 2) prod *= a2pow;
        e /= 2;
        a2pow *= a2pow;
    }
    return prod;
}

int evaltree(node *p, int *err) {
    int val = 0, lval = 0, rval = 0;
    if (p == NULL) {
        fprintf(stderr, "Error: Empty expression tree\n");
        if (err) *err = 1;
        return 0;
    }
    if ((p->type == ID) || (p->type == NUM)) {
        val = getvalue(p->data, err);
    } else if (p->type == OP) {
        lval = evaltree(p->L, err);
        rval = evaltree(p->R, err);
        switch (p->data) {
            case ADD: val = lval + rval; break;
            case SUB: val = lval - rval; break;
            case MUL: val = lval * rval; break;
            case DIV:
                if (rval == 0) {
                    fprintf(stderr, "Error: Division by zero\n");
                    if (err) *err = 1;
                    val = 0;
                } else {
                    val = lval / rval;
                }
                break;
            case REM:
                if (rval == 0) {
                    fprintf(stderr, "Error: Modulo by zero\n");
                    if (err) *err = 1;
                    val = 0;
                } else {
                    val = lval % rval;
                }
                break;
            case POW: val = intpow(lval, rval); break;
            default:
                fprintf(stderr, "Error: Unknown opcode %d\n", p->data);
                if (err) *err = 1;
                val = 0;
                break;
        }
    } else {
        fprintf(stderr, "Error: Unknown node type in expression tree\n");
        if (err) *err = 1;
        val = 0;
    }
    free(p);
    return val;
}

/* Lexer and Parser for (set ID val) and (op arg arg) */
typedef enum {
    TOK_LP,
    TOK_RP,
    TOK_SET,
    TOK_OP_ADD,
    TOK_OP_SUB,
    TOK_OP_MUL,
    TOK_OP_DIV,
    TOK_OP_REM,
    TOK_OP_POW,
    TOK_ID_STR,
    TOK_NUM_STR,
    TOK_EOF_OR_ERR
} ParseToken;

static const char *parse_ptr;

static void skip_spaces() {
    while (*parse_ptr == ' ' || *parse_ptr == '\t' || *parse_ptr == '\r' || *parse_ptr == '\n') parse_ptr++;
}

static ParseToken next_token(char *out_val) {
    skip_spaces();
    if (*parse_ptr == '\0') return TOK_EOF_OR_ERR;

    if (*parse_ptr == '(') {
        parse_ptr++;
        return TOK_LP;
    }
    if (*parse_ptr == ')') {
        parse_ptr++;
        return TOK_RP;
    }

    if (*parse_ptr == '*' && *(parse_ptr + 1) == '*') {
        parse_ptr += 2;
        return TOK_OP_POW;
    }
    if (*parse_ptr == '^') {
        parse_ptr++;
        return TOK_OP_POW;
    }
    if (*parse_ptr == '+') {
        parse_ptr++;
        return TOK_OP_ADD;
    }
    if (*parse_ptr == '-' && !isdigit((unsigned char)*(parse_ptr + 1))) {
        parse_ptr++;
        return TOK_OP_SUB;
    }
    if (*parse_ptr == '*') {
        parse_ptr++;
        return TOK_OP_MUL;
    }
    if (*parse_ptr == '/') {
        parse_ptr++;
        return TOK_OP_DIV;
    }
    if (*parse_ptr == '%') {
        parse_ptr++;
        return TOK_OP_REM;
    }

    if (isdigit((unsigned char)*parse_ptr) || (*parse_ptr == '-' && isdigit((unsigned char)*(parse_ptr + 1)))) {
        int i = 0;
        if (*parse_ptr == '-') out_val[i++] = *parse_ptr++;
        while (isdigit((unsigned char)*parse_ptr)) {
            out_val[i++] = *parse_ptr++;
        }
        out_val[i] = '\0';
        return TOK_NUM_STR;
    }

    if (isalpha((unsigned char)*parse_ptr) || *parse_ptr == '_') {
        int i = 0;
        while (isalnum((unsigned char)*parse_ptr) || *parse_ptr == '_') {
            out_val[i++] = *parse_ptr++;
        }
        out_val[i] = '\0';
        if (strcmp(out_val, "set") == 0) return TOK_SET;
        return TOK_ID_STR;
    }

    return TOK_EOF_OR_ERR;
}

static node *parse_expr_internal(int *err);

static node *parse_arg_internal(int *err) {
    const char *save = parse_ptr;
    char token_text[128];
    ParseToken tok = next_token(token_text);
    if (tok == TOK_NUM_STR) {
        return addleaf(addsymbol(NUM, token_text));
    }
    if (tok == TOK_ID_STR) {
        return addleaf(addsymbol(ID, token_text));
    }
    if (tok == TOK_LP) {
        parse_ptr = save; // rewind to '('
        return parse_expr_internal(err);
    }
    *err = 1;
    return NULL;
}

static node *parse_expr_internal(int *err) {
    char token_text[128];
    ParseToken tok = next_token(token_text);
    if (tok != TOK_LP) {
        *err = 1;
        return NULL;
    }

    tok = next_token(token_text);
    int opcode = 0;
    switch (tok) {
        case TOK_OP_ADD: opcode = ADD; break;
        case TOK_OP_SUB: opcode = SUB; break;
        case TOK_OP_MUL: opcode = MUL; break;
        case TOK_OP_DIV: opcode = DIV; break;
        case TOK_OP_REM: opcode = REM; break;
        case TOK_OP_POW: opcode = POW; break;
        default:
            *err = 1;
            return NULL;
    }

    node *left = parse_arg_internal(err);
    if (*err || !left) return NULL;
    node *right = parse_arg_internal(err);
    if (*err || !right) return NULL;

    tok = next_token(token_text);
    if (tok != TOK_RP) {
        *err = 1;
        return NULL;
    }

    return addnode(opcode, left, right);
}

void process_line(const char *line) {
    parse_ptr = line;
    skip_spaces();
    if (*parse_ptr == '\0') return;

    char token_text[128];
    const char *save_start = parse_ptr;
    ParseToken tok = next_token(token_text);

    if (tok != TOK_LP) {
        fprintf(stderr, "Error: syntax error\n");
        return;
    }

    const char *after_lp = parse_ptr;
    tok = next_token(token_text);

    if (tok == TOK_SET) {
        // (set ID NUM|ID|expr )
        tok = next_token(token_text);
        if (tok != TOK_ID_STR) {
            fprintf(stderr, "Error: syntax error\n");
            return;
        }
        char var_name[64];
        strcpy(var_name, token_text);
        int sym_idx = addsymbol(ID, var_name);

        const char *arg_save = parse_ptr;
        tok = next_token(token_text);
        int eval_err = 0;
        int value = 0;

        if (tok == TOK_NUM_STR) {
            value = atoi(token_text);
            tok = next_token(token_text);
            if (tok != TOK_RP) {
                fprintf(stderr, "Error: syntax error\n");
                return;
            }
        } else if (tok == TOK_ID_STR) {
            value = getsymbol(token_text, &eval_err);
            tok = next_token(token_text);
            if (tok != TOK_RP || eval_err) {
                if (!eval_err) fprintf(stderr, "Error: syntax error\n");
                return;
            }
        } else if (tok == TOK_LP) {
            parse_ptr = arg_save;
            node *tree = parse_expr_internal(&eval_err);
            if (eval_err || !tree) {
                fprintf(stderr, "Error: syntax error\n");
                return;
            }
            tok = next_token(token_text);
            if (tok != TOK_RP) {
                fprintf(stderr, "Error: syntax error\n");
                return;
            }
            value = evaltree(tree, &eval_err);
            if (eval_err) return;
        } else {
            fprintf(stderr, "Error: syntax error\n");
            return;
        }

        setsymbol(sym_idx, value);
        printf("Variable %s is set to %d\n", ST[sym_idx].data, ST[sym_idx].value);
    } else {
        // Standalone expression: (op arg arg)
        parse_ptr = save_start;
        int eval_err = 0;
        node *tree = parse_expr_internal(&eval_err);
        if (eval_err || !tree) {
            fprintf(stderr, "Error: syntax error\n");
            return;
        }
        int val = evaltree(tree, &eval_err);
        if (!eval_err) {
            printf("Standalone expression evaluates to %d\n", val);
        }
    }
}

int main(void) {
    char line[512];
    while (fgets(line, sizeof(line), stdin)) {
        process_line(line);
    }
    return 0;
}
