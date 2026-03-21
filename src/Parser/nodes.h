#ifndef NODES_H
#define NODES_H

#include <stdbool.h>

typedef enum {
    OT_PLUS,
    OT_MINUS,
    OT_MUL,
    OT_POW,
    OT_DIV,
    OT_IDIV,
    OT_MOD
} OperationType;

typedef enum {
    NT_STATEMENTS,
    NT_RETURN,
    NT_BREAK,
    NT_CONTINUE,
    NT_VAR_DECLARE,
    NT_VAR_ASSIGN,
    NT_BIN_OP,
    NT_NUMBER
} NodeType;


typedef struct BNode Node;

typedef struct {
    size_t len;
    size_t cap;
    Node** nodes;
} NodeArray;

typedef struct BNode {
    NodeType nt;
    bool cc;
    union {
        NodeArray StatementsNode;
        NodeArray ReturnNode;
        struct { Node* value; TValue var_name; } VarDeclareNode;
        struct { Node* value; TValue var_name; } VarAssignNode;
        struct { Node* left; Node* right; OperationType op; } BinOpNode;
        struct { TValue num; } NumberNode;
    } node;
} Node;


#endif