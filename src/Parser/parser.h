#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#include "Error/error.h"
#include "Lexer/lexer.h"
#include "Parser/nodes.h"


typedef struct {
    Token* tokens;
    Error* err;
    Node* node;
    size_t adv_count;
} ParseResult;


Node* parse(Token* tokens, char filename[]);
Node* statement(ParseResult* res);
Node* statements(ParseResult* res);

static inline void clean_newlines(Token** tokens, size_t* adv_count);
static inline Node* register_node(Node* node, ParseResult* res);
static inline ParseResult new_res(Token* tokens);
static inline Node* new_node(NodeType nt);

static NodeArray make_node_array();
static Node* append_to_node_array(NodeArray* nda, Node* node);

#endif