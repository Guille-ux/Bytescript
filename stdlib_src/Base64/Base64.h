#ifndef TBASE64_H
#define TBASE64_H

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

extern const char base64_encode_table[];
extern char base64_decode_table[256];

TValue encode_base64(TValue* argv, size_t argc);
TValue decode_base64(TValue* argv, size_t argc);

#endif