#ifndef DISASSMBLER
#define DISASSMBLER

#include "Compiler/instructions.h"
#include "Values/bytev.h"

int get_instr_jump_lenght(const uint8_t* instr);
void disassemble(TValue tfunc);
void disassemble_constants(TValue tfunc);
void full_disassemble(TValue tfunc);

#endif