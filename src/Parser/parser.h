#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#include "Values/helper_structs.h"
#include "Lexer/lexer.h"
#include "Parser/nodes.h"


#define SEPARATOR       ((Token){ .tt = TT_SEPARATOR, .value = NIL })
#define EOF_TOKEN       ((Token){ .tt = TT_EOF, .value = NIL })
#define END_TOKEN       ((Token){ .tt = TT_KWORD, .value = make_fstring("end") })
#define WHILE_TOKEN     ((Token){ .tt = TT_KWORD, .value = make_fstring("while") })
#define ELSEIF_TOKEN    ((Token){ .tt = TT_KWORD, .value = make_fstring("elseif") })
#define EXPECTED(exc)   { (exc); if (res->err_details) return NULL; }


typedef struct {
    Token* tokens;
    char* err_details;
    size_t adv_count;
    TTable* user_types;
} ParseResult;

typedef Node* (*ParseFunc)(ParseResult* res);

static Node* bin_op(ParseResult* res, ParseFunc func1, Token ops[], int opc, ParseFunc func2);
static NodeArray separated(ParseResult* res, ParseFunc func, Token separator);
static Node* postfix_op(ParseResult* res, Node* num);

Node* parse(Token* tokens, char filename[]);
Node* alloc_expr(ParseResult* res);
Node* if_expr(ParseResult* res);
Node* for_expr(ParseResult* res);
Node* while_expr(ParseResult* res);
Node* repeat_expr(ParseResult* res);
Node* import_expr(ParseResult* res);
Node* func_expr(ParseResult* res);
Node* numerics(ParseResult* res);
Node* postfix(ParseResult* res);
Node* power(ParseResult* res);
Node* factor(ParseResult* res);
Node* term(ParseResult* res);
Node* arithmetic(ParseResult* res);
Node* comparaison(ParseResult* res);
Node* logical(ParseResult* res);
Node* expr(ParseResult* res);
Node* statement(ParseResult* res);
Node* statements(ParseResult* res);
Node* get_statements(ParseResult* res, Token stopToken);

static inline void clean_newlines(Token** tokens, size_t* adv_count);
static inline ParseResult new_res(Token* tokens);
static inline Node* new_node(NodeType nt);

static inline void clean_node_array(NodeArray* nda, size_t start_index);
static NodeArray make_node_array();
static Node* append_to_node_array(NodeArray* nda, Node* node);

static inline bool is_correct_operator(Token op, Token ops[], int opc);
static void expect(ParseResult* res, TokenType tt, char* expect, TValue* value, char* kword);
TValue type_specifier(ParseResult* res);

extern Token LOGICAL_OPERATORS[];
extern Token COMP_OPERATORS[];
extern Token ARITHMETIC_OPERATORS[];
extern Token TERM_OPERATORS[];
extern Token FACTOR_OPERATORS[];
extern Token POWER_OPERATORS[];
extern size_t LOGICAL_OPERATORS_SIZE;
extern size_t COMP_OPERATORS_SIZE;
extern size_t ARITHMETIC_OPERATORS_SIZE;
extern size_t TERM_OPERATORS_SIZE;
extern size_t FACTOR_OPERATORS_SIZE;
extern size_t POWER_OPERATORS_SIZE;
void init_operators();

#endif