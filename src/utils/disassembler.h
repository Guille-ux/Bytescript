#ifndef DISASSMBLER
#define DISASSMBLER

#include "Compiler/compiler.h"

int get_jump_lenght(Scope* context, uint8_t* instr);
void disassemble(Scope* context);

#endif