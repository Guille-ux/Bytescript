#ifndef DISASSMBLER
#define DISASSMBLER

#include "Compiler/instructions.h"
#include "Values/bytev.h"

int get_instr_jump_lenght(const uint8_t* instr);
TValue dis(TValue tfunc);
TValue disassemble_constants(TValue tfunc);
TValue full_disassemble(TValue tfunc);

#endif