#ifndef LABELER_H
#define LABELER_H

#include <stdint.h>
#include "Values/tutils.h" 

static void patch_single_jfalse_label(uint8_t* label, int32_t pos) {
    *(label - 2) = INT32_TO_HI(pos);
    *(label - 1) = INT32_TO_LO(pos);
}

static void patch_single_jump_label(uint8_t* label, int32_t pos) {
    *(label - 3) = INT32_TO_HIHI(pos);
    *(label - 2) = INT32_TO_HI(pos);
    *(label - 1) = INT32_TO_LO(pos);
}

struct ArrayLabel { uint8_t** ptr; size_t len; };
static void patch_array_jump_label(struct ArrayLabel label, uint8_t* pos) {
    for (int i = 0; i < label.len; i++) {
        uint8_t* lab = label.ptr[i];
        patch_single_jump_label(lab, (pos - lab) / 4);
    }
}

static inline int get_jump_lenght_to(uint8_t* label, Scope* context, uint8_t* target) { return (target - label) / 4; }
static inline int get_jump_lenght(uint8_t* label, Scope* context) { return get_jump_lenght_to(label, context, context->bytecode.bytes + context->bytecode.len); }

#endif