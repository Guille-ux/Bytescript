#ifndef PARSER_H
#define PARSER_H

#include "Error/error.h"
#include "Lexer/lexer.h"
#include "Parser/nodes.h"


typedef struct {
    Token* tokens;
    Error* err;
    Node* node;
    size_t adv_count;
} ParseResult;


Node* parse(Token* tokens);
Node* statement(ParseResult* res);
Node* statements(ParseResult* res);

static inline void clean_newlines(Token** tokens, size_t* adv_count);
static inline void register_node(Node* node, ParseResult* res);
static inline ParseResult new_res(Token* tokens);
static inline Node* new_node(NodeType nt);

#endif