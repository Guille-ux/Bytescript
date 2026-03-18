#include "lexer.h"
#include "Error/error.h"

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>


static inline bool next_match(char* text, char next_match) {return *++text == next_match;}

static inline void emit(Token* tp, TokenType tt, TValue value) {
    Token tk = (Token){.tt = tt, .value = value};
    *tp = tk;
}

static inline void double_case(char* text, Token* tp, char next_case, TokenType case1, TokenType case2) {
    if (next_match(text, next_case)) emit(tp, case1, NIL); else emit(tp, case2, NIL);
}



static inline void make_word(Token* tp, char** text) {
    char* pst_text = *text;
    char c = **text;
    size_t len = 1;

    while (isalpha((c = *++*text)) || c == '_') len++;
    if (is_keyword(pst_text, len)) emit(tp, TT_KWORD, make_substring(pst_text, len));
    else emit(tp, TT_IDENTIFIER, make_substring(pst_text, len));
}

static inline void emit_default(Token* tp, char** text) {
    if (isdigit(**text) || **text == '.') return emit(tp, TT_NUMBER, make_number(text));
    else if (isalpha(**text) || **text == '_') return make_word(tp, text);
    else {
        char details[snprintf(NULL, 0, "Unidentified character : %c\n\tCannot Tokenize", **text) + 1];
        snprintf(details, sizeof(details), "Unidentified character : %c\n\tCannot Tokenize", **text);
        call_error(new_error("UnidentifiedCharacter Error", details, 0, 0), NULL);
    }
}

TValue make_number(char** text) {
    double res = strtod(*text, text);
    return NUM_AS_TVALUE(res);
}

TValue make_string_token(char** text, char initializer) {
    size_t cap = 16;
    size_t len = 0;
    char* buff = malloc(cap);
    char* pre_text = *text;

    if (buff == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making string");
    while (*++*text != initializer && **text != '\0') {
         if (**text == '\\') {
            switch (*++*text) {
                case 'n': {
                    buff[len] = '\n';
                    break;
                }

                case 't': {
                    buff[len] = '\t';
                    break;
                }
                
                default: goto normal_char;
            }

        } else {
            normal_char:
            buff[len] = **text;
        }

        len++;
        if (len >= cap) {
            char* new = realloc(buff, (cap *= 2));
            if (new == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making string");

            buff = new;
        }
    }

    TValue str =  make_substring(buff, len);
    free(buff);
    return str;
}


Token* make_tokens(char* text, size_t text_len, size_t* tok_len) {
    size_t cap = 32;
    Token* tokens = malloc(sizeof(Token) * cap);
    Token* tp = tokens;
    *tok_len = 0;

    if (tokens == NULL) FAILED_ALLOCATION_ERROR("Failed allocation of token table");

    do {
        if (*text == ' ' || *text == '\t') goto skip;

        switch (*text) {
            case '+': {
                emit(tp, TT_PLUS, NIL);
                break;
            }

            case '-': {
                emit(tp, TT_MINUS, NIL);
                break;
            }

            case '*': {
                double_case(text, tp, '*', TT_MUL, TT_POW);
                break;
            }

            case '/': {
                double_case(text, tp, '/', TT_DIV, TT_IDIV);
                break;
            }

            case '%': {
                emit(tp, TT_MOD, NIL);
                break;
            }

            case '=': {
                emit(tp, TT_EQ, NIL);
                break;
            }

            case '.': {
                if (*(text + 1) == '.') emit(tp, TT_CONCAT, NIL);
                else emit(tp, TT_NUMBER, make_number(&text));
                break;
            }

            case '"': {
                emit(tp, TT_STR, make_string_token(&text, '"'));
                break;
            }

            case '\'': {
                emit(tp, TT_STR, make_string_token(&text, '\''));
                break;
            }

            case '\n': {
                emit(tp, TT_NEWLINE, NIL);
                break;
            }

            case '\0': {
                goto end;
                break;
            }
            
            default: {
                emit_default(tp, &text);
                break;
            }
        }

        tp++;
        if (++*tok_len >= cap) {
            size_t current = tokens - tp;
            cap *= 2;

            tokens = realloc(tokens, sizeof(Token) * cap);
            if (tokens == NULL) FAILED_ALLOCATION_ERROR("Failed allocation of token table");

            tp = tokens + current;
        }

        skip:
    } while (text++);

    end:
    return tokens;
}