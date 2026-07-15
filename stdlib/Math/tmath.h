#ifndef TMATH_H
#define TMATH_H

#include "Math/math_utils.h"
#include "Values/bytev.h"

extern char bytescript_module_map[];
TValue bytescript_module_loader(TValue* argv, size_t argc);

#endif