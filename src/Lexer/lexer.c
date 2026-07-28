#include "lexer.h"
#include "Globals/error.h"
#include "Globals/string.h"

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>


inline bool match_keyword(Token tok, char kword[]) {
    return match_token(tok, (Token){ .tt = TT_KWORD, .value = make_fstring(kword) });
}

inline bool match_tt(Token tok1, BTokenType tt) {
    return tok1.tt == tt;
}

inline bool match_token(Token tok1, Token tok2) {
    return match_tt(tok1, tok2.tt) && AS_TVALUE(tok1.value) == AS_TVALUE(tok2.value);
}

static inline bool next_match(char* text, char next_match) {return *++text == next_match;}

static inline void emit_token(Token* tp, BTokenType tt, TValue value) {
    Token tk = (Token){.tt = tt, .value = value};
    *tp = tk;
}

static inline void double_case(char** text, Token* tp, char next_case, BTokenType case1, BTokenType case2) {
    if (next_match(*text, next_case)) {emit_token(tp, case2, NIL); (*text)++;} else emit_token(tp, case1, NIL);
}

static inline void triple_case(char** text, Token* tp, char next_case, char next_case2, BTokenType case1, BTokenType case2, BTokenType case3) {
    if (next_match(*text, next_case)) {emit_token(tp, case2, NIL); (*text)++;}
    else if (next_match(*text, next_case2)) {emit_token(tp, case3, NIL); (*text)++;}
    else emit_token(tp, case1, NIL);
}



static inline void make_word(Token* tp, char** text) {
    char* pst_text = *text;
    char c = **text;
    size_t len = 1;

    while (isalpha((c = *++*text)) || isdigit(**text) || c == '_') len++;
    if (is_keyword(pst_text, len)) {
        TValue keyword = make_string(pst_text, len);
        TStr* keyword_str = AS_STRING(keyword);

        if (!strcmp("not", keyword_str->str)) emit_token(tp, TT_NOT, NIL);
        else if (!strcmp("and", keyword_str->str)) emit_token(tp, TT_RAND, NIL);
        else if (!strcmp("or", keyword_str->str)) emit_token(tp, TT_ROR, NIL);
        else emit_token(tp, TT_KWORD, keyword);

    } else emit_token(tp, TT_IDENTIFIER, make_string(pst_text, len));
    *--*text;
}

static inline void emit_default(Token* tp, char** text) {
    if (isdigit(**text) || **text == '.') return emit_token(tp, TT_NUMBER, make_number(text));
    else if (isalpha(**text) || **text == '_') return make_word(tp, text);
    else {
        TValue err = make_fstring("Unidentified character : %c\n\tCannot Tokenize");
        err = format_string((TValue[2]){ err, **text }, 2);
        call_error(new_unidentified_character_error, AS_STRING(err)->str);
    }
}

TValue make_number(char** text) {
    double res = strtod(*text, text);
    *--*text;

    return NUM_AS_TVALUE(res);
}

TValue make_string_token(char** text, char initializer) {
    size_t cap = 16;
    size_t len = 0;
    char* buff = malloc(cap);
    char* pre_text = *text;

    if (buff == NULL) call_error(new_failed_allocation_error, "Failed allocation while making string");
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
            if (new == NULL) call_error(new_failed_allocation_error, "Failed allocation while making string");

            buff = new;
        }
    }

    TValue str =  make_string(buff, len);
    free(buff);

    if (!(*text)) --*text;
    return str;
}


Token* make_tokens(char* text) {
    size_t cap = 32;
    Token* tokens = malloc(sizeof(Token) * cap);
    Token* tp = tokens;
    size_t tok_len = 0;

    if (tokens == NULL) call_error(new_failed_allocation_error, "Failed allocation of token table");

    do {
        if (*text == ' ' || *text == '\t' || *text == '\r') goto skip;

        switch (*text) {
            case '+': {
                emit_token(tp, TT_PLUS, NIL);
                break;
            }

            case '-': {
                if (next_match(text, '-')) {
                    while (*++text && *text != '\n');
                    break;
                }

                double_case(&text, tp, '>', TT_MINUS, TT_ARROW);
                break;
            }

            case '*': {
                double_case(&text, tp, '*', TT_MUL, TT_POW);
                break;
            }

            case '/': {
                double_case(&text, tp, '/', TT_DIV, TT_IDIV);
                break;
            }

            case '%': {
                emit_token(tp, TT_MOD, NIL);
                break;
            }

            case '=': {
                double_case(&text, tp, '=', TT_EQ, TT_EEQ);
                break;
            }

            case '.': {
                if (*(text + 1) == '.') {emit_token(tp, TT_CONCAT, NIL); text++;}
                else if (isdigit(*(text + 1))) (tp, TT_NUMBER, make_number(&text));
                else emit_token(tp, TT_DOT, NIL);
                break;
            }

            case '"': {
                emit_token(tp, TT_STR, make_string_token(&text, '"'));
                break;
            }

            case '\'': {
                emit_token(tp, TT_STR, make_string_token(&text, '\''));
                break;
            }

            case '(': {
                emit_token(tp, TT_LPAR, NIL);
                break;
            }

            case ')': {
                emit_token(tp, TT_RPAR, NIL);
                break;
            }

            case '[': {
                emit_token(tp, TT_LSBRAC, NIL);
                break;
            }

            case ']': {
                emit_token(tp, TT_RSBRAC, NIL);
                break;
            }

            case '{': {
                emit_token(tp, TT_LCBRAC, NIL);
                break;
            }

            case '}': {
                emit_token(tp, TT_RCBRAC, NIL);
                break;
            }

            case '!': {
                double_case(&text, tp, '=', TT_NOT, TT_NEQ);
                break;
            }

            case '<': {
                triple_case(&text, tp, '=', '<', TT_LT, TT_LTEQ, TT_LSHIFT);
                break;
            }

            case '>': {
                triple_case(&text, tp, '=', '>', TT_GT, TT_GTEQ, TT_RSHIFT);
                break;
            }

            case '&': {
                double_case(&text, tp, '&', TT_BAND, TT_AND);
                break;
            }

            case '|': {
                double_case(&text, tp, '|', TT_BOR, TT_OR);
                break;
            }

            case '^': {
                emit_token(tp, TT_BXOR, NIL);
                break;
            }

            case ',': {
                emit_token(tp, TT_SEPARATOR, NIL);
                break;
            }

            case ':': {
                emit_token(tp, TT_COLON, NIL);
                break;
            }

            case ';':

            case '\n': {
                emit_token(tp, TT_NEWLINE, NIL);
                break;
            }

            case EOF:

            case '\0': {
                emit_token(tp, TT_EOF, NIL);
                ++tok_len;
                goto end;
                break;
            }
            
            default: {
                emit_default(tp, &text);
                break;
            }
        }

        tp++;
        if (++tok_len >= cap) {
            cap *= 2;
            tokens = realloc(tokens, sizeof(Token) * cap);
            if (tokens == NULL) call_error(new_failed_allocation_error, "Failed allocation of token table");

            tp = tokens + tok_len;
        }

        skip:
    } while (text++);

    end:
    return tokens;
}