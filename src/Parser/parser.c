#include <stdlib.h>

#include "Globals/string.h"
#include "Parser/parser.h"



Node* parse(Token* tokens) {
    ParseResult res = new_res(tokens);
}

Node* statement(ParseResult* res) {
    
}

Node* statements(ParseResult* res) {
    Node* node = new_node(STATEMENTS_NODE);

    do {
        clean_newlines(&res->tokens, &res->adv_count);
        Node* stmnt = statement(res);

        if (stmnt == NULL || res->err != NULL) break;
        register_node(stmnt, res);
    } while ((*res->tokens).tt != TT_EOF);

    return node;
}


static inline void clean_newlines(Token** tokens, size_t* adv_count) { while ( (**tokens).tt == TT_NEWLINE && ++*adv_count ) ++*tokens; }
static inline void register_node(Node* node, ParseResult* res) { res->node = node; }
static inline ParseResult new_res(Token* tokens) { return (ParseResult){ .err = NULL, .node = NULL, .adv_count = 0, .tokens = tokens }; }
static inline Node* new_node(NodeType nt) { Node* node =  malloc(sizeof(Node)); node->nt = nt; }