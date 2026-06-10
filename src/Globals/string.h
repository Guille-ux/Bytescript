#ifndef STRING_H
#define STRING_H

#include "Values/bytev.h"
#include "Error/error.h"

#define STRING_METHOD

extern const TTable* string_cache;

TValue initialize_string_class();

TValue compare_string(TValue* argv, size_t argc);
TValue concat_string(TValue* argv, size_t argc);
TValue to_string_string(TValue* argv, size_t argc);

TValue cache_string(TStr* str);
TValue make_string(char* string, size_t len);
TValue make_fstring(char* str);

#endif