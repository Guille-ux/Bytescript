#ifndef TABLE_H
#define TABLE_H

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"


#define WITHIN_SCOPE(v, i)          (i < v->len || i >= 0)
#define OUT_OF_SCOPE(v, i)          (i > v->len - 1 || i < 0)


TValue new_table(TValue* argv, uint8_t argc);
TValue free_table(TValue* argv, uint8_t argc);
TValue pop_table(TValue* argv, uint8_t argc);
TValue shrink_table(TValue* argv, uint8_t argc);
TValue expand_table(TValue* argv, uint8_t argc);
TValue len_table(TValue* argv, uint8_t argc);
TValue reserve_table(TValue* argv, uint8_t argc);
TValue print_table(TValue* argv, uint8_t argc);
TValue slice_table(TValue* argv, uint8_t argc);
TValue fill_table(TValue* argv, uint8_t argc);
TValue copy_table(TValue* argv, uint8_t argc);
TValue insert_table(TValue* argv, uint8_t argc);


#endif