/*
================================================================================
EX NO: 9  23Z511 – SYSTEM SOFTWARE LABORATORY
evalexpr.c - Predictive parsing for parenthesized arithmetic expressions,
parse tree generation, symbol table management, and tree evaluation.
================================================================================
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#ifndef EXPR
#define EXPR 1001
#define OP 1002
#define ARG 1003
#define ID 1004
#define NUM 1005
#define LP 1006
#define RP 1007
#define ADD 1008
#define SUB 1009
#define MUL 1010
#define DIV 1011
#define REM 1012
#define INV 1013
#endif

// Macro to check whether a token is a binary operator
#define binop(x) ( (x == ADD) || (x == SUB) || (x == MUL) || (x == DIV) || (x == REM) )

// Structure representing a node in the symbol table 
typedef struct _stnode {
    char *id;            // Stores the identifier name
    int value;           // Stores the value of the identifier
    struct _stnode *next;// Points to the next symbol table node
} stnode;

// Defines symbtbl as a pointer to a symbol table node 
typedef stnode *symbtbl;

// Structure representing a constant number 
typedef struct _ctnode {
    int value;           // Stores the integer value
    struct _ctnode *next;// Points to the next constant node
} ctnode;

// Defines constbl as a pointer to a constant node 
typedef ctnode *constbl;

// Structure representing a stack node 
typedef struct _stacknode {
    int item;                // Stores the item in the stack
    struct _stacknode *next; // Points to the next stack node
} stacknode;

// Defines stack as a pointer to a stack node 
typedef stacknode *stack;

// Structure representing a node in the parse tree 
typedef struct _treenode {
    int type; // Stores the type of the tree node
    // Union stores operator, identifier or number information 
    union {
        char *op;    // Stores an operator
        stnode *id;  // Points to an identifier node 
        ctnode *num; // Points to a number node
    } ref;
    struct _treenode *L; // Pointer to left child 
    struct _treenode *R; // Pointer to right child 
    struct _treenode *P; // Pointer to parent
} treenode;

// Defines parsetree as a pointer to a tree node 
typedef treenode *parsetree;

#ifdef COMPILE_WITH_FLEX
#include "lex.yy.c"
#else
extern int yylex(void);
extern char *yytext;
#endif

// Function to insert an identifier into the symbol table 
symbtbl insertid(symbtbl T, char *id, stnode **ref)
{
    stnode *p;
    // Start searching from the beginning of the symbol table 
    p = T;
    // Search for the identifier in the existing symbol table 
    while (p != NULL) {
        if (!strcmp(p->id, id)) {
            *ref = p; // Return the existing identifier node 
            return T;
        }
        p = p->next;
    }
    // Create a new symbol table node if identifier does not exist 
    p = (stnode *)malloc(sizeof(stnode));
    // Allocate memory for the identifier string
    p->id = (char *)malloc((1 + strlen(id)) * sizeof(char));
    // Copy the identifier into the new node 
    strcpy(p->id, id);
    // Initialize the identifier value to zero
    p->value = 0;
    // Insert the new node at the beginning of the table 
    p->next = T;
    // Return the address of the newly created node
    *ref = p; 
    return p;
}

// Function to insert a number into the constant table 
constbl insertnum(constbl C, char *num, ctnode **ref)
{
    ctnode *p;
    // Allocate memory for a new constant node 
    p = (ctnode *)malloc(sizeof(ctnode));
    // Convert the string number into an integer 
    p->value = atoi(num);
    // Insert the new node at the beginning of the table 
    p->next = C;
    // Return the address of the new constant node
    *ref = p; 
    return p;
}

// Function to return the top element of the stack 
int top(stack S)
{
    // Check whether the stack is empty 
    if (S == NULL) {
        fprintf(stderr, "*** Read from an empty stack\n"); 
        exit(1);
    }
    // Return the item at the top of the stack 
    return S->item;
}

// Function to push an item onto the stack
stack push(stack S, int item)
{
    stacknode *p;
    // Allocate memory for a new stack node
    p = (stacknode *)malloc(sizeof(stacknode));
    // Store the item in the new node 
    p->item = item;
    // Link the new node to the existing stack 
    p->next = S;
    // Return the new top of the stack 
    return p;
}

// Function to remove the top item from the stack 
stack pop(stack S)
{
    stacknode *p;
    // Check whether the stack is empty 
    if (S == NULL) {
        fprintf(stderr, "*** Pop from an empty stack\n"); 
        exit(1);
    }
    // Store the current top node 
    p = S;
    // Move the stack pointer to the next node 
    S = S->next;
    // Free the removed node 
    free(p);
    // Return the updated stack 
    return S;
}

/*
The following parse() function implements the textbook 
predictive parsing algorithm under an LL(1) grammar.

The predictive parsing table contains the following non-error entries:
  M[EXPR,LP]   = EXPR -> ( OP ARG ARG )
  M[OP,binop]  = OP -> binop
  M[ARG,ID]    = ARG -> ID 
  M[ARG,NUM]   = ARG -> NUM 
  M[ARG,LP]    = ARG -> ( EXPR )
*/

// Function to perform predictive parsing and construct the parse tree 
parsetree parse(symbtbl *T, constbl *C)
{
    int A, a;               // A = stack symbol, a = current input token 
    stack S = NULL;         // Initialize parsing stack
    parsetree PT = NULL;    // Initialize parse tree
    treenode *p = NULL, *q; // Pointers used while creating the tree

    // Push the starting grammar symbol EXPR onto the stack 
    S = push(S, EXPR);
    // Get the first token from the lexical analyzer 
    a = yylex();

    // Continue parsing until the stack becomes empty 
    while (1) {
        // Stop when there are no more symbols in the stack 
        if (S == NULL)
            break;
        // Get and remove the top symbol from the stack 
        A = top(S);
        S = pop(S);

        // Handle the EXPR non-terminal 
        if (A == EXPR) {
            // EXPR must begin with a left parenthesis 
            if (a != LP) {
                fprintf(stderr, "*** Error: LP expected in place of %s\n", yytext); 
                exit(2);
            }
            // Push the right-hand side of EXPR -> ( OP ARG ARG ) onto the stack in reverse order 
            S = push(S, RP);
            S = push(S, ARG); 
            S = push(S, ARG);
            S = push(S, OP);
            S = push(S, LP);
        }
        // Handle the ARG non-terminal 
        else if (A == ARG) {
            // ARG can be an identifier 
            if (a == ID)
                S = push(S, ID);
            // ARG can be a number 
            else if (a == NUM)
                S = push(S, NUM);
            // ARG can be another expression 
            else if (a == LP)
                S = push(S, EXPR);
            // Report an error for an invalid argument 
            else {
                fprintf(stderr, "*** Error: ID/NUM/LP expected in place of %s\n", yytext); 
                exit(3);
            }
        }
        // Handle the OP non-terminal 
        else if (A == OP) {
            // Check whether the current token is a valid operator 
            if (!binop(a)) {
                fprintf(stderr, "*** Error: Invalid operator %s found\n", yytext); 
                exit(4);
            }
            // Push the operator token onto the stack
            S = push(S, a);
        }
        // Handle terminal symbols 
        else {
            // Handle an identifier token 
            if (A == ID) {
                // The current input token must be an identifier 
                if (a != ID) {
                    fprintf(stderr, "*** ERROR: ID expected in place of %s\n", yytext); 
                    exit(5);
                }
                // Check whether the current parent can have another child 
                if ((p == NULL) || ((p->L != NULL) && (p->R != NULL))) {
                    fprintf(stderr, "*** Error: Invalid id %s found\n", yytext); 
                    exit(6);
                }
                // Create the left or right child node 
                if (p->L == NULL)
                    q = p->L = (treenode *)malloc(sizeof(treenode)); 
                else
                    q = p->R = (treenode *)malloc(sizeof(treenode));
                // Mark the node as an identifier node 
                q->type = ID;
                // Insert the identifier into the symbol table
                *T = insertid(*T, yytext, &((q->ref).id));
                // Initialize child pointers 
                q->L = q->R = NULL;
                // Set the parent pointer 
                q->P = p;
            }
            // Handle a number token 
            else if (A == NUM) {
                // The current input token must be a number
                if (a != NUM) {
                    fprintf(stderr, "*** ERROR: Number expected in place of %s\n", yytext); 
                    exit(7);
                }
                // Check whether the current parent can have another child 
                if ((p == NULL) || ((p->L != NULL) && (p->R != NULL))) {
                    fprintf(stderr, "*** Error: Invalid number %s found\n", yytext); 
                    exit(8);
                }
                // Create the left or right child node 
                if (p->L == NULL)
                    q = p->L = (treenode *)malloc(sizeof(treenode)); 
                else
                    q = p->R = (treenode *)malloc(sizeof(treenode));
                // Mark the node as a number node 
                q->type = NUM;
                // Insert the number into the constant table
                *C = insertnum(*C, yytext, &((q->ref).num));
                // Initialize child pointers 
                q->L = q->R = NULL;
                // Set the parent pointer 
                q->P = p;
            }
            // Handle the left parenthesis 
            else if (A == LP) {
                // Check whether the current token is a left parenthesis 
                if (a != LP) {
                    fprintf(stderr, "*** Error: Left parenthesis expected in place of %s\n", yytext); 
                    exit(9);
                }
            }
            // Handle the right parenthesis 
            else if (A == RP) {
                // Check whether the current token is a right parenthesis 
                if (a != RP) {
                    fprintf(stderr, "*** Error: Right parenthesis expected in place of %s\n", yytext); 
                    exit(10);
                }
                // Move to the parent until a node without a right child is found or the root node is reached 
                while ((p != PT) && (p->R != NULL))
                    p = p->P;
            }
            // Handle binary operator tokens 
            else if (binop(A)) {
                // Create a new operator node 
                if (PT == NULL) {
                    // Create the root node of the parse tree
                    p = PT = (treenode *)malloc(sizeof(treenode));
                    // Root node has no parent 
                    PT->P = NULL;
                } else {
                    // Add the new operator as the left child if possible 
                    if (p->L == NULL)
                        q = p->L = (treenode *)malloc(sizeof(treenode));
                    // Otherwise add it as the right child 
                    else if (p->R == NULL)
                        q = p->R = (treenode *)malloc(sizeof(treenode));
                    // Report an error if both children already exist 
                    else {
                        fprintf(stderr, "*** Error: Cannot create subtree for operator %s (%d)\n", yytext, p->type); 
                        exit(11);
                    }
                    // Set the parent pointer 
                    q->P = p;
                    // Make the new operator node the current parent 
                    p = q;
                }
                // Mark the node as an operator node 
                p->type = OP;
                // Allocate memory for the operator string
                (p->ref).op = (char *)malloc((1 + strlen(yytext)) * sizeof(char));
                // Copy the operator into the tree node 
                strcpy((p->ref).op, yytext);
                // Initialize child pointers 
                p->L = p->R = NULL;
            }

            // The current input token has been consumed.
            // Read the next token if the stack is not empty. 
            if (S != NULL)
                a = yylex();
        }
    }

    // Check whether there are extra characters in the input 
    if (S != NULL) {
        fprintf(stderr, "*** Error: Extra characters in input\n"); 
        exit(12);
    }
    // Display successful parsing message 
    printf("Parsing is successful\n");
    // Return the constructed parse tree 
    return PT;
}

// Function to print the parse tree 
void prntree(parsetree PT, int level)
{
    int l;
    // Return if the tree is empty 
    if (PT == NULL)
        return;
    // Print indentation according to the tree level 
    for (l = 0; l < level - 1; ++l)
        printf("   ");
    // Print an arrow for non-root nodes 
    if (level)
        printf(" ---> ");
    // Print operator node 
    if (PT->type == OP)
        printf("OP(%s)", (PT->ref).op);
    // Print identifier node 
    else if (PT->type == ID)
        printf("ID(%s)", (PT->ref).id->id);
    // Print number node
    else if (PT->type == NUM)
        printf("NUM(%d)", (PT->ref).num->value); 
    printf("\n");
    // Recursively print the left subtree 
    prntree(PT->L, level + 1);
    // Recursively print the right subtree 
    prntree(PT->R, level + 1);
}

// Function to print all identifiers in the symbol table 
void prnsymbols(symbtbl T)
{
    // Traverse the symbol table 
    while (T) {
        // Print the identifier 
        printf("\"%s\"\n", T->id);
        // Move to the next symbol 
        T = T->next;
    }
}

// Function to read values for all identifiers
void readvalues(symbtbl T)
{
    int tok;
    // Stop if the symbol table is empty 
    if (T == NULL)
        return;
    // Recursively process the remaining symbols 
    readvalues(T->next);
    // Read the next token from the input 
    tok = yylex();
    // The value must be a number 
    if (tok != NUM) {
        fprintf(stderr, "*** Error: %s is not an integer value for %s\n", yytext, T->id); 
        exit(13);
    }
    // Convert the input string to an integer 
    T->value = atoi(yytext);
    // Display the identifier and its assigned value 
    printf("%s = %d\n", T->id, T->value);
}

// Function to evaluate the parse tree 
int evaltree(parsetree PT)
{
    // Return zero if the tree is empty 
    if (PT == NULL)
        return 0;
    // Evaluate according to the node type 
    switch (PT->type) {
        // Return the value stored for an identifier 
        case ID:
            return ((PT->ref).id)->value;
        // Return the value of a number 
        case NUM:
            return ((PT->ref).num)->value;
        // Evaluate an operator node 
        case OP:
            // Perform addition
            if (!strcmp((PT->ref).op, "+"))
                return evaltree(PT->L) + evaltree(PT->R);
            // Perform subtraction
            if (!strcmp((PT->ref).op, "-"))
                return evaltree(PT->L) - evaltree(PT->R);
            // Perform multiplication
            if (!strcmp((PT->ref).op, "*"))
                return evaltree(PT->L) * evaltree(PT->R);
            // Perform division
            if (!strcmp((PT->ref).op, "/"))
                return evaltree(PT->L) / evaltree(PT->R);
            // Perform remainder operation 
            if (!strcmp((PT->ref).op, "%"))
                return evaltree(PT->L) % evaltree(PT->R);
    }
    return 0;
}

#ifdef COMPILE_MAIN
// Main function 
int main()
{
    parsetree PT;     // Stores the parse tree
    symbtbl T = NULL; // Initializes the symbol table 
    constbl C = NULL; // Initializes the constant table

    // Parse the input and construct the parse tree 
    PT = parse(&T, &C);

    // Print the generated parse tree 
    prntree(PT, 0);

    // If identifiers are present, read their values 
    if (T) {
        printf("Reading variable values from the input\n");
        readvalues(T);
    }

    // Evaluate and display the final result
    printf("The expression evaluates to %d\n", evaltree(PT));

    // Terminate the program successfully 
    exit(0);
}
#endif
