#ifndef NODES_H
#define NODES_H


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
    STATEMENTS_NODE,
    NUMBER_NODE,
    BIN_OP_NODE,
    VAR_DECLARE_NODE,
    VAR_ASSIGN_NODE
} NodeType;


typedef struct BNode {
    NodeType nt;
    union {
        struct { size_t len; struct BNode* nodes; } StatementsNode;
        struct { TValue num; } NumberNode;
        struct { struct BNode* left; struct BNode* right; OperationType op; } BinOpNode;
        struct { struct BNode* value; TValue var_name; } VarDeclareNode;
        struct { struct BNode* value; TValue var_name; } VarAssignNode;
    } node;
} Node;


#endif