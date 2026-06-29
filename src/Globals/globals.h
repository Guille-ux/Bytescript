#ifndef GLOBALS_H
#define GLOBALS_H

#include "Values/bytev.h"
#include "Values/namespaces.h"
#include "Values/tutils.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Globals/tError.h"

#define NEW_METHOD(name, classname)     { make_fstring(snake_to_camel_case((char[]){ #name }, sizeof(#name))), new_cfunc((CFunc)name##_##classname) }
#define NEW_FIELD(name, value)          { make_fstring(snake_to_camel_case((char[]){ name }, sizeof(name))), value }
#define MIN_ARGC(n)                     { if (argc < n) return NIL; }
#define STRICT_MIN_ARGC(n, fname)       { if (argc < n) call_error(new_invalid_args_error, "function <" fname "> take " #n " arguments"); }

extern TTable* globals_namespace;

TValue print(TValue* argv, size_t argc);
TValue tfree(TValue* argv, size_t argc);

void initialize_globals();
void append_methods_to_map(TValue map, TValue fields[][2], TValue fieldvalues[], size_t fields_size);

#endif