#ifndef UTILS
#define UTILS

#include "Values/bytev.h"


TValue shift_tvalue_array(TValue arr[], size_t size, int start);
uint8_t shift_uint8_array(uint8_t arr[], size_t size, int start);
char* snake_to_camel_case(char* snaked, size_t len);
int has_modifier(TValue tbl, char modif[]);
TValue dunder(TValue str);
TValue undunder(TValue str);
TValue is_dunder(TValue tstr);

#endif