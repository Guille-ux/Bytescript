#ifndef NODES_H
#define NODES_H

#include "Lexer/lexer.h"
#include <stdbool.h>



typedef enum {
    NT_STATEMENTS,
    NT_RETURN,
    NT_BREAK,
    NT_CONTINUE,
    NT_VAR_DECLARE,
    NT_ASSIGNEMENT,
    NT_BIN_OP,
    NT_UNI_OP,
    NT_CALL,
    NT_VAR_ACESS,
    NT_NUMBER,
    NT_STRING,
    NT_PLACEHOLDER
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
        struct { Node* value; Node* lvalue; } AssignementNode;
        struct { Node* left; Node* right; Token op; } BinOpNode;
        struct { Node* target; Token op; } UniOpNode;
        struct { Node* target; NodeArray args; } CallNode;
        struct { TValue name; } VarAcess;
        struct { TValue num; } NumberNode;
        struct { TValue str; } StringNode;
    } node;
} Node;


#endif