#ifndef BUILTIN_H
#define BUILTIN_H

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"
#include "Globals/table.h"


TValue print(TValue* argv, size_t argc);
TValue tfree(TValue* argv, size_t argc);


#endif