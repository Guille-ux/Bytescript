#ifndef GLOBALS_H
#define GLOBALS_H

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"
#include "Globals/table.h"

#define NEW_METHOD(name, classname)     { make_fstring(snake_to_camel_case((char[]){ #name }, sizeof(#name))), new_cfunc((CFunc*)name##_##classname) }

extern TTable* globals_maps;
extern TTable* globals_namespace;

TValue print(TValue* argv, size_t argc);
TValue tfree(TValue* argv, size_t argc);

void initialize_globals();

#endif