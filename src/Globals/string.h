#ifndef STRING_H
#define STRING_H

#include "Values/bytev.h"
#include "Error/error.h"


extern const TTable* string_cache;


TValue cache_string(char* string, size_t len);
TValue make_string(char* string, size_t len);
TValue make_fstring(char* str);


TValue compare_string(TValue* argv, size_t argc);

#endif