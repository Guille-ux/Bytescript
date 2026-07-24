#ifndef TJSON_H
#define TJSON_H

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

TValue parse_json(TValue* argv, size_t argc);
TValue stringify_json(TValue* argv, size_t argc);
TValue valid_json(TValue* argv, size_t argc);
TValue load_json(TValue* argv, size_t argc);
TValue write_json(TValue* argv, size_t argc);

TValue parse_json_value(char** str);
TValue parse_json_object(char** str);
TValue parse_json_array(char** str);

TValue make_string_json(char** text, char initializer);
TValue get_next_word(char** str);
void next_value(char** str);

#endif