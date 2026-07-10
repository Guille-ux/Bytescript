#ifndef LABELER_H
#define LABELER_H

#include <stdint.h>
#include "Values/tutils.h"

#define CURRENT_INSTR (context->bytecode.len)

static inline int get_jump_lenght_to(size_t label, size_t target) { return (target - label) / 4; }
static inline int get_jump_lenght(size_t label, Scope* context) { return get_jump_lenght_to(label, CURRENT_INSTR); }

static void patch_single_jfalse_label(uint8_t* bytecode, size_t label, int32_t pos) {
    *((bytecode + label) - 2) = INT32_TO_HI(pos);
    *((bytecode + label) - 1) = INT32_TO_LO(pos);
}

static void patch_single_jump_label(uint8_t* bytecode, size_t label, int32_t pos) {
    *((bytecode + label) - 3) = INT32_TO_HIHI(pos);
    *((bytecode + label) - 2) = INT32_TO_HI(pos);
    *((bytecode + label) - 1) = INT32_TO_LO(pos);
}

struct ArrayLabel { size_t len; size_t* ptr; };
static void patch_array_jump_label(uint8_t* bytecode, struct ArrayLabel label, size_t pos) {
    for (int i = 0; i < label.len; i++) {
        size_t lab = label.ptr[i];
        patch_single_jump_label(bytecode, lab, get_jump_lenght_to(lab, pos));
    }
}

#endif