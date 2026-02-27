#ifndef TABLE
#define TABLE

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"


TValue free_table(TValue* argv, uint8_t argc);
TValue pop_table(TValue* argv, uint8_t argc);
TValue len_table(TValue* argv, uint8_t argc);
TValue reserve(TValue* argv, uint8_t argc);
TValue print_table(TValue* argv, uint8_t argc);
TValue slice_table(TValue* argv, uint8_t argc);
TValue fill_table(TValue* argv, uint8_t argc);
TValue copy_table(TValue* argv, uint8_t argc);
TValue insert_table(TValue* argv, uint8_t argc);


#endif