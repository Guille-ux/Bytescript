#include <stdlib.h>

#include "Globals/string.h"
#include "Parser/parser.h"



Node* parse(Token* tokens, char filename[]) {
    ParseResult res = new_res(tokens);

    register_node(statements(&res), &res);
    if (res.err != NULL) call_error(res.err, filename);
    return res.node;
}

Node* statement(ParseResult* res) {
    if (match_keyword(*res->tokens, "return")) {

    } else if (match_keyword(*res->tokens, "break")) return new_node(NT_BREAK);
    else if (match_keyword(*res->tokens, "continue")) return new_node(NT_CONTINUE);

    return NULL;
}

Node* statements(ParseResult* res) {
    Node* node = new_node(NT_STATEMENTS);
    NodeArray statementNodes = make_node_array();

    do {
        clean_newlines(&res->tokens, &res->adv_count);
        Node* stmnt = register_node(statement(res), res);

        //printf("%d\n", stmnt == NULL);
        if (stmnt == NULL || res->err != NULL) break;
        append_to_node_array(&statementNodes, stmnt);
        
    } while ((*res->tokens).tt != TT_EOF);

    node->node.StatementsNode = statementNodes;
    return node;
}


static inline void clean_newlines(Token** tokens, size_t* adv_count) { while ( (**tokens).tt == TT_NEWLINE && ++*adv_count ) ++*tokens; }
static inline Node* register_node(Node* node, ParseResult* res) { res->node = node; return node; }
static inline ParseResult new_res(Token* tokens) { return (ParseResult){ .err = NULL, .node = NULL, .adv_count = 0, .tokens = tokens }; }
static inline Node* new_node(NodeType nt) { Node* node =  malloc(sizeof(Node)); node->nt = nt; return node; }

static NodeArray make_node_array() {
    Node** ndarray = malloc(sizeof(Node*) * 8);
    if (ndarray == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making node");
    return (NodeArray){ .len = 0, .cap = 8, .nodes = ndarray };
}

static Node* append_to_node_array(NodeArray* nda, Node* node) {
    if (nda->len >= nda->cap) {
        nda->nodes = realloc(nda->nodes, sizeof(Node*) * (nda->cap *= 2));
        if (nda->nodes == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making node");
    }

    nda->nodes[nda->len++] = node;
}