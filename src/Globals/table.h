#ifndef TABLE_H
#define TABLE_H

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"
#include "Globals/builtins.h"
#include "rapidhash.h"


#define WITHIN_SCOPE(v, i)          (i < v->array.len || i >= 0)
#define OUT_OF_SCOPE(v, i)          (i >= v->array.len  || i < 0)


static inline uint64_t hash_single_value(TValue value);

TValue new_table(TValue* argv, size_t argc);
TValue free_table(TValue* argv, size_t argc);
TValue pop_table(TValue* argv, size_t argc);
TValue shrink_table(TValue* argv, size_t argc);
TValue expand_table(TValue* argv, size_t argc);
TValue len_table(TValue* argv, size_t argc);
TValue reserve_table(TValue* argv, size_t argc);
TValue print_table(TValue* argv, size_t argc);
TValue slice_table(TValue* argv, size_t argc);
TValue fill_table(TValue* argv, size_t argc);
TValue copy_table(TValue* argv, size_t argc);
TValue insert_table(TValue* argv, size_t argc);
TValue hash_value_table(TValue* argv, size_t argc);


#endif