#ifndef TMATH_H
#define TMATH_H

#include "Math/math_utils.h"
#include "Values/bytev.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

extern TValue pi_math;
extern TValue e_math;
extern TValue inf_math;

TValue max_math(TValue* argv, size_t argc);
TValue min_math(TValue* argv, size_t argc);
TValue clamp_math(TValue* argv, size_t argc);
TValue abs_math(TValue* argv, size_t argc);
TValue floor_math(TValue* argv, size_t argc);
TValue ceil_math(TValue* argv, size_t argc);
TValue round_math(TValue* argv, size_t argc);
TValue trunc_math(TValue* argv, size_t argc);
TValue sqrt_math(TValue* argv, size_t argc);
TValue sin_math(TValue* argv, size_t argc);
TValue cos_math(TValue* argv, size_t argc);
TValue tan_math(TValue* argv, size_t argc);
TValue asin_math(TValue* argv, size_t argc);
TValue acos_math(TValue* argv, size_t argc);
TValue atan_math(TValue* argv, size_t argc);
TValue atan2_math(TValue* argv, size_t argc);
TValue exp_math(TValue* argv, size_t argc);
TValue log_math(TValue* argv, size_t argc);
TValue log10_math(TValue* argv, size_t argc);
TValue sign_math(TValue* argv, size_t argc);
TValue is_inf_math(TValue* argv, size_t argc);
TValue deg_math(TValue* argv, size_t argc);
TValue rad_math(TValue* argv, size_t argc);

#endif