#include "utils/disassembler.h"

const char* opcodes_names[] = {
    [OP_ADD_RC] = "ADD_RC",
    [OP_ADD_RR] = "ADD_RR",
    [OP_SUB_RC] = "SUB_RC",
    [OP_SUB_CR] = "SUB_CR",
    [OP_SUB_RR] = "SUB_RR",
    [OP_MUL_RC] = "MUL_RC",
    [OP_MUL_RR] = "MUL_RR",
    [OP_DIV_RC] = "DIV_RC",
    [OP_DIV_CR] = "DIV_CR",
    [OP_DIV_RR] = "DIV_RR",
    [OP_IDIV_RC] = "IDIV_RC",
    [OP_IDIV_CR] = "IDIV_CR",
    [OP_IDIV_RR] = "IDIV_RR",
    [OP_MOD_RC] = "MOD_RC",
    [OP_MOD_CR] = "MOD_CR",
    [OP_MOD_RR] = "MOD_RR",
    [OP_POW_RC] = "POW_RC",
    [OP_POW_CR] = "POW_CR",
    [OP_POW_RR] = "POW_RR",
    [OP_CONCAT_RC] = "CONCAT_RC",
    [OP_CONCAT_CR] = "CONCAT_CR",
    [OP_CONCAT_RR] = "CONCAT_RR",
    [OP_EEQ_RC] = "EEQ_RC",
    [OP_EEQ_RR] = "EEQ_RR",
    [OP_NEQ_RC] = "NEQ_RC",
    [OP_NEQ_RR] = "NEQ_RR",
    [OP_LT_RC] = "LT_RC",
    [OP_LT_RR] = "LT_RR",
    [OP_GT_RC] = "GT_RC",
    [OP_GT_RR] = "GT_RR",
    [OP_LTEQ_RC] = "LTEQ_RC",
    [OP_LTEQ_RR] = "LTEQ_RR",
    [OP_GTEQ_RC] = "GTEQ_RC",
    [OP_GTEQ_RR] = "GTEQ_RR",
    [OP_RAND_RC] = "RAND_RC",
    [OP_RAND_RR] = "RAND_RR",
    [OP_ROR_RC] = "ROR_RC",
    [OP_ROR_RR] = "ROR_RR",
    [OP_AND_RC] = "AND_RC",
    [OP_AND_RR] = "AND_RR",
    [OP_OR_RC] = "OR_RC",
    [OP_OR_RR] = "OR_RR",
    [OP_BAND_RC] = "BAND_RC",
    [OP_BAND_RR] = "BAND_RR",
    [OP_BOR_RC] = "BOR_RC",
    [OP_BOR_RR] = "BOR_RR",
    [OP_BXOR_RC] = "BXOR_RC",
    [OP_BXOR_RR] = "BXOR_RR",
    [OP_BNOT] = "BNOT",
    [OP_JUMP] = "JUMP",
    [OP_JFALSE] = "JFALSE",
    [OP_NFOR_INIT] = "NFOR_INIT",
    [OP_NFOR_LOOP] = "NFOR_LOOP",
    [OP_GFOR_INIT] = "GFOR_INIT",
    [OP_GFOR_LOOP] = "GFOR_LOOP",
    [OP_LOAD_CR] = "LOAD_CR",
    [OP_LOAD_RR] = "LOAD_RR",
    [OP_LOAD_UP] = "LOAD_UP",
    [OP_CALL] = "CALL",
    [OP_RETURN] = "RETURN",
    [OP_NEWTABLE] = "NEWTABLE",
    [OP_ADDTABLE] = "ADDTABLE",
    [OP_GETTABLE] = "GETTABLE",
    [OP_SETTABLE] = "SETTABLE",
    [OP_HALT] = "HALT",
};

void disassemble(Scope* context) {
    // collecting labels

    int* labels = malloc(sizeof(int) * 64);
    for (int i = 0; i < context->bytecode.len; i+= 4) {
        uint8_t op = context->bytecode.bytes[i];

        if (op = OP_JUMP || op == OP_JFALSE || op == OP_GFOR_LOOP || op == OP_NFOR_LOOP) {
            
        }
    }

    // printing

    printf("\ndisassembling :\n");
    for (int i = 0; i < context->bytecode.len; i+= 4) {
        printf("%s %d %d %d\n", opcodes_names[context->bytecode.bytes[i]], context->bytecode.bytes[i + 1], context->bytecode.bytes[i + 2], context->bytecode.bytes[i + 3]);
    }
    printf("\n");

    free(labels);
}