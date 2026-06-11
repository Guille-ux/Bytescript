#ifndef NODES_H
#define NODES_H

#include "Values/helper_structs.h"
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
    NT_ALLOCATION,
    NT_IF,
    NT_NFOR,
    NT_GFOR,
    NT_WHILE,
    NT_REPEAT,
    NT_IMPORT,
    NT_FUNCDEF,
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
    TValue type;
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
        struct { Node* target; NodeArray args; } AllocNode;
        struct { NodeArray cases; NodeArray statements; } IfNode;
        struct { TValue identifier; Node* values[3]; Node* statements; } nForNode;
        struct { TValue identifiers[2]; Node* target; Node* statements; } gForNode;
        struct { Node* condition; Node* statements; } whileNode;
        struct { Node* condition; Node* statements; } repeatNode;
        struct { TValue module; TValue identifier; } importNode;
        struct { TValue name; struct TValueLinkedList args; Node* statements; } funcDefNode;
    } node;
} Node;


static inline int node_equal(Node* node1, Node* node2) {
    if (node1->nt != node2->nt) return 0;
    switch (node1->nt) {
        case NT_STATEMENTS: {
            if (node1->node.StatementsNode.len != node2->node.StatementsNode.len) return 0;
            for (int i = 0; i < node1->node.StatementsNode.len; i++) { if (!node_equal(node1->node.StatementsNode.nodes[i], node2->node.StatementsNode.nodes[i])) return 0; }

            return 1;
        }

        case NT_RETURN: {
            if (node1->node.ReturnNode.len != node2->node.ReturnNode.len) return 0;
            for (int i = 0; i < node1->node.ReturnNode.len; i++) { if (!node_equal(node1->node.ReturnNode.nodes[i], node2->node.ReturnNode.nodes[i])) return 0; }

            return 1;
        }
        
        case NT_BREAK: return 1;
        case NT_CONTINUE: return 1;
        case NT_VAR_DECLARE: return AS_TVALUE(node1->node.VarDeclareNode.var_name) == AS_TVALUE(node2->node.VarDeclareNode.var_name) &&
                                    node_equal(node1->node.VarDeclareNode.value, node2->node.VarDeclareNode.value);

        case NT_ASSIGNEMENT: return node_equal(node1->node.AssignementNode.lvalue, node2->node.AssignementNode.lvalue) &&
                                    node_equal(node1->node.AssignementNode.value, node2->node.AssignementNode.value);

        case NT_BIN_OP: return  node1->node.BinOpNode.op.tt = node2->node.BinOpNode.op.tt &&
                                node_equal(node1->node.BinOpNode.left, node2->node.BinOpNode.left) &&
                                node_equal(node1->node.BinOpNode.right, node2->node.BinOpNode.right);

        case NT_UNI_OP: return  node1->node.UniOpNode.op.tt = node2->node.UniOpNode.op.tt &&
                                node_equal(node1->node.UniOpNode.target, node2->node.UniOpNode.target);

        case NT_CALL: return 0;
        case NT_VAR_ACESS: return AS_TVALUE(node1->node.VarAcess.name) == AS_TVALUE(node2->node.VarAcess.name);
        case NT_NUMBER: return AS_TVALUE(node1->node.NumberNode.num) == AS_TVALUE(node2->node.NumberNode.num);
        case NT_STRING: return AS_TVALUE(node1->node.StringNode.str) == AS_TVALUE(node2->node.StringNode.str);
    }

    return 0;
}


#endif