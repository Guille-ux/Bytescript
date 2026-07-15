#define _GNU_SOURCE
#include <math.h>

#include "Math/tmath.h"
#include "src/Globals/globals.h"

__declspec(dllexport) char* bytescript_module_map = "tmath_map.bs";
__declspec(dllexport) TValue bytescript_module_loader(TValue* argv, size_t argc) {
    
}


TValue pi_math = NUM_AS_TVALUE(M_PI);
TValue e_math = NUM_AS_TVALUE(M_E);
TValue inf_math = NUM_AS_TVALUE(INFINITY);

TValue max_math(TValue* argv, size_t argc) {
    MIN_ARGC(2);
    return NUM_AS_TVALUE(MAX(argv[0].num, argv[1].num));
}

TValue min_math(TValue* argv, size_t argc) {
    MIN_ARGC(2);
    return NUM_AS_TVALUE(MIN(argv[0].num, argv[1].num));
}

TValue clamp_math(TValue* argv, size_t argc) {
    MIN_ARGC(3);
    return NUM_AS_TVALUE(CLAMP(argv[0].num, argv[1].num, argv[2].num));
}

TValue abs_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(abs(argv[0].num));
}

TValue floor_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(floor(argv[0].num));
}

TValue ceil_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(ceil(argv[0].num));
}

TValue round_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(round(argv[0].num));
}

TValue trunc_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(trunc(argv[0].num));
}

TValue sqrt_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(sqrt(argv[0].num));
}

TValue sin_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(sin(argv[0].num));
}

TValue cos_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(cos(argv[0].num));
}

TValue tan_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(tan(argv[0].num));
}

TValue asin_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(asin(argv[0].num));
}

TValue acos_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(acos(argv[0].num));
}

TValue atan_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(atan(argv[0].num));
}

TValue atan2_math(TValue* argv, size_t argc) {
    MIN_ARGC(2);
    return NUM_AS_TVALUE(atan2(argv[0].num, argv[1].num));
}

TValue exp_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(exp(argv[0].num));
}

TValue log_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(log(argv[0].num));
}

TValue log10_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(log10(argv[0].num));
}

TValue sign_math(TValue* argv, size_t argc) {

}

TValue is_inf_math(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    return NUM_AS_TVALUE(EQUAL(inf_math, argv[0]));
}

TValue deg_math(TValue* argv, size_t argc) {

}

TValue rad_math(TValue* argv, size_t argc) {

}
