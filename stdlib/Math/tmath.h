#ifndef TMATH_H
#define TMATH_H

#include "Math/math_utils.h"
#include "Values/bytev.h"

char bytescript_module_map[];
__declspec(dllexport) TValue bytescript_module_loader(TValue* argv, size_t argc);

#endif