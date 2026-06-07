#ifndef DISASSMBLER
#define DISASSMBLER

#include "Compiler/compiler.h"

int get_instr_jump_lenght(const uint8_t* instr);
void disassemble(TValue tfunc);
void disassemble_constants(TValue tfunc, size_t len);

#endif