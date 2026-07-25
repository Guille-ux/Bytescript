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
TValue add_tab(TValue str);
TValue add_to_res(TValue* res, TValue value);
TValue stringify_json_pair(TValue key, TValue value, size_t tabs, size_t* hashed_dones);
TValue stringify_json_value(TValue value, size_t tabs);
void next_value(char** str);

#endif