#include "lexer.h"
#include <stdio.h>

const char* keywords[] = {
    "var",

    "and",
    "or",
    "not",

    "if",
    "then",
    "elseif",
    "else",

    "for",
    "while",
    "repeat",
    "break",
    "continue",

    "in",

    "static",
    "class",
    "func",
    "return",
    
    "alloc",

    "import",
    "as",
    
    "end"
};


inline bool is_keyword(char* str, size_t len) {
    char buff[len + 1];
    strncpy(buff, str, len);

    buff[len] = '\0';
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (!strcmp(keywords[i], buff)) return true;
    }

    return false;
}