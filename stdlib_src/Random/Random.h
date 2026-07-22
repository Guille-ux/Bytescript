#ifndef TRANDOM_H
#define TRANDOM_H

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

TValue seed_random(TValue* argv, size_t argc);
TValue random_random(TValue* argv, size_t argc);
TValue range_random(TValue* argv, size_t argc);
TValue integer_random(TValue* argv, size_t argc);

#endif