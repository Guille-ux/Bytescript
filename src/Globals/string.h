#ifndef STRING_H
#define STRING_H

#include "Values/bytev.h"
#include "Error/error.h"

#define STRING_METHOD

typedef struct {
    uint8_t flags;
    int width;
    int precision;
    char specifier;
} FormatInfos;

enum FormatFlags {
    FORMAT_FLAG_ZERO = 1 << 0,
    FORMAT_FLAG_SPACE= 1 << 1,
    FORMAT_FLAG_SIGN = 1 << 2,
    FORMAT_FLAG_LEFT = 1 << 3,
};

extern const TTable* string_cache;
extern uint8_t flag_flags[];
extern char valid_flags[];

TValue initialize_string_class();

TValue compare_string(TValue* argv, size_t argc);
TValue concat_string(TValue* argv, size_t argc);
TValue to_string_string(TValue* argv, size_t argc);
TValue format_string(TValue* argv, size_t argc);

TValue cache_string(TStr* str);
TValue make_string(char* string, size_t len);
TValue make_fstring(char* str);

TValue format_s(FormatInfos infos, TValue value);
TValue format_n(FormatInfos infos, TValue value);

#endif