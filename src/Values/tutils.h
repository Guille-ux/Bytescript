#ifndef TUTILS_H
#define TUTILS_H

#include "Values/bytev.h"
#include "Globals/table.h"
#include "Globals/builtins.h"
#include "Lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HIHI_TO32_T(hihi)       ( ( (int32_t)(hihi) << 16 ) )
#define HI_TO32_T(hi)           ( ( (int16_t)(hi) << 8 ) )
#define LO_TO32_T(lo)           ( (int8_t)(lo) )

#define OFF24_TO_INT32(hihi, hi, lo)                                      \
    ( ((HIHI_TO32_T(hihi) | HI_TO32_T(hi) | LO_TO32_T(lo))) |             \
      (((hihi) & 0x80) ? 0xFF000000 : 0) )


TValue to_return_value(TValue returnv[], size_t returnc);
char* read_string();
void print_tokens(Token* tokens);

#endif