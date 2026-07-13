#ifndef BVM_H
#define BVM_H

#include "Values/bytev.h"
#include <stdio.h>

#define STACK_FRAME_SIZE    (BSIZE(256))
#define STACK_SIZE          (BSIZE(STACK_FRAME_SIZE))

#define CONSTANT(idx)       ( frame.func->bytescriptFunction.constPool[idx] )
#define NEXT_INSTR          { frame.ip += 4; arg1 = frame.ip[1]; arg2 = frame.ip[2]; arg3 = frame.ip[3]; goto *dispatch_table[frame.ip[0]]; }

struct TSFrame;
typedef struct TSFrame {
    struct TSFrame* prev;
    const uint8_t* ip;
    TFunc* func;
} StackFrame;

typedef struct {
    StackFrame frame;
    TValue* sp;
} VMState;

TValue vm_run(TValue func);
static TValue run_function(TValue func, VMState context);
static inline TValue vm_module_load(VMState context, uint8_t arg2, uint8_t arg3, TValue* return_ptr);

#endif