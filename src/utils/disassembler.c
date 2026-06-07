#include "utils/disassembler.h"
#include "Globals/table.h"
#include "Globals/string.h"

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

int get_instr_jump_lenght(const uint8_t* instr) {
    uint8_t op = instr[0];

    uint8_t hi = instr[2];
    uint8_t lo = instr[3];
    uint8_t hihi = op != OP_JUMP ? (int16_t)(hi << 8) >> 8 : instr[1];

    return off24_to_int32(hihi, hi, lo);
}

void disassemble(TValue tfunc) {
    // collecting labels

    TFunc* func = AS_FUNC(tfunc);
    const uint8_t* bytecode = func->bytescriptFunction.bytecode;

    TValue L = make_fstring("L");
    TValue labels = new_table(NULL, 0);
    int current_label_number = 0;

    while (*bytecode != OP_HALT) {
        int i = bytecode - func->bytescriptFunction.bytecode;
        uint8_t op = *bytecode;

        if (op == OP_JUMP || op == OP_JFALSE || op == OP_GFOR_LOOP || op == OP_NFOR_LOOP || op == OP_NFOR_INIT || op == OP_GFOR_INIT) {
            int jump_lenght = get_instr_jump_lenght(bytecode);
            TValue cached = get_value_from_hashtable(labels, NUM_AS_TVALUE(i / 4 + jump_lenght + 1));
            TValue label_name = IS_NIL(cached) ? to_string(( TValue[2] ){ L, NUM_AS_TVALUE(current_label_number++) }, 2) : cached;

            add_value_to_hashtable(labels, NUM_AS_TVALUE(i / 4+ jump_lenght + 1), label_name);
        }

        bytecode += 4;
    }

    // printing

    printf("\ndisassembling :\n\n");

    bytecode = func->bytescriptFunction.bytecode;
    while (*bytecode != OP_HALT) {
        int i = bytecode - func->bytescriptFunction.bytecode;
        uint8_t instr = *bytecode;
        uint8_t arg1 = bytecode[1];
        uint8_t arg2 = bytecode[2];
        uint8_t arg3 = bytecode[3];

        const char* opcode = opcodes_names[instr];
        TValue current_label = get_value_from_hashtable(labels, NUM_AS_TVALUE(i / 4));

        if (!IS_NIL(current_label)) printf("\n%s\n", AS_STRING(current_label)->str);

        printf("%-12s", opcode);
        if (instr == OP_JUMP || instr == OP_JFALSE || instr == OP_GFOR_LOOP || instr == OP_NFOR_LOOP || instr == OP_NFOR_INIT || instr == OP_GFOR_INIT) {
            int jump_lenght = get_instr_jump_lenght(bytecode);
            TValue target_label = get_value_from_hashtable(labels, NUM_AS_TVALUE(i / 4 + jump_lenght + 1));
            
            if (instr == OP_JUMP) printf("%s\n", AS_STRING(target_label)->str);
            else printf("%d %s\n", arg1, AS_STRING(target_label)->str);

        } else printf("%d %d %d\n", arg1, arg2, arg3);

        bytecode += 4;
    }

    printf("%-12s %d %d %d\n\n", opcodes_names[*bytecode], bytecode[1], bytecode[2], bytecode[3]);
}

void disassemble_constants(TValue tfunc) {
    printf("disassembling constants :\n\n");

    TFunc* func = AS_FUNC(tfunc);
    size_t len = func->bytescriptFunction.const_len;
    TValue buffer[len];
    memcpy(buffer, func->bytescriptFunction.constPool, BSIZE(len));

    print(buffer, len);
    printf("\n");
}

void full_disassemble(TValue tfunc) {
    disassemble(tfunc);
    disassemble_constants(tfunc);

    TFunc* func = AS_FUNC(tfunc);
    for (int i = 0; i < func->bytescriptFunction.const_len; i++) {
        TValue v = func->bytescriptFunction.constPool[i];
        if (TYPE(v) == FUNC) {
            printf("anonymous");
            full_disassemble(v);
        }
    }
}