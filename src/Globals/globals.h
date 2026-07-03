#ifndef GLOBALS_H
#define GLOBALS_H

#include "Values/bytev.h"
#include "Values/namespaces.h"
#include "Values/tutils.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Globals/tError.h"

#define MIN_ARGC(n)                     { if (argc < n) return NIL; }
#define STRICT_MIN_ARGC(n, fname)       { if (argc < n) call_error(new_invalid_args_error, "function <" fname "> take " #n " arguments"); }

extern TTable* globals_namespace;
extern TValue globals_map;

TValue print(TValue* argv, size_t argc);
TValue tfree(TValue* argv, size_t argc);

void initialize_globals();

#endif