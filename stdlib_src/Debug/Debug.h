#ifndef TDEBUG_H
#define TDEBUG_H

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

TValue new_assertion_error(TValue* argv, size_t argc) ;
TValue assert_debug(TValue* argv, size_t argc);

#endif