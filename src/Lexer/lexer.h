#ifndef LEXER_H
#define LEXER_H

#include "Values/bytev.h"
#include <stdbool.h>
#include <string.h>


typedef enum {
    TT_NUMBER,
    TT_STR,
    TT_PLUS,
    TT_MINUS,
    TT_MUL,
    TT_POW,
    TT_DIV,
    TT_IDIV,
    TT_MOD,
    TT_CONCAT,
    TT_RPAR,
    TT_LPAR,
    TT_RCBRAC,
    TT_LCBRAC,
    TT_RSBRAC,
    TT_LSBRAC,
    TT_EQ,
    TT_EEQ,
    TT_NEQ,
    TT_LT,
    TT_GT,
    TT_LTEQ,
    TT_GTEQ,
    TT_AND,
    TT_OR,
    TT_BAND,
    TT_BOR,
    TT_BXOR,
    TT_LSHIFT,
    TT_RSHIFT,
    TT_IDENTIFIER,
    TT_KWORD,
    TT_SEPARATOR,
    TT_ARROW,
    TT_NEWLINE,
    TT_UNDEF,
    TT_EOF
} TokenType;

typedef struct {
    TokenType tt;
    TValue value;
} Token;



extern const char* keywords[];
bool is_keyword(char* str, size_t len);

static inline bool next_match(char* text, char next_match);
static inline void emit(Token* tp, TokenType tt, TValue value);
static inline void double_case(char* text, Token* tp, char next_case, TokenType case1, TokenType case2);

static inline void make_word(Token* tp, char** text);
static inline void emit_default(Token* tp, char** text);
TValue make_number(char** text);
TValue make_string_token(char** text, char initializer);

Token* make_tokens(char* text, size_t text_len, size_t* tok_len);
void print_tokens(Token* tokens, size_t tok_len);


#endif