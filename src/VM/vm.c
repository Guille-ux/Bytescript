#include "Compiler/instructions.h"
#include "Values/tutils.h"
#include "VM/vm.h"
#include <math.h>


TValue vm_run(TValue func) {
    TValue* stack = malloc(STACK_SIZE);
    VMState context = { .sp = stack, .frame = NULL };
    TValue return_value = run_function(func, context);

    return return_value;
}


static TValue run_function(TValue func, VMState context) {
    static void* dispatch_table[] = {
        [OP_ADD_RC] = 		&&instr_add_rc,
        [OP_ADD_RR] = 		&&instr_add_rr,
        [OP_SUB_RC] = 		&&instr_sub_rc,
        [OP_SUB_CR] = 		&&instr_sub_cr,
        [OP_SUB_RR] = 		&&instr_sub_rr,
        [OP_MUL_RC] = 		&&instr_mul_rc,
        [OP_MUL_RR] = 		&&instr_mul_rr,
        [OP_DIV_RC] = 		&&instr_div_rc,
        [OP_DIV_CR] = 		&&instr_div_cr,
        [OP_DIV_RR] = 		&&instr_div_rr,
        [OP_IDIV_RC] = 		&&instr_idiv_rc,
        [OP_IDIV_CR] = 		&&instr_idiv_cr,
        [OP_IDIV_RR] = 		&&instr_idiv_rr,
        [OP_MOD_RC] = 		&&instr_mod_rc,
        [OP_MOD_CR] = 		&&instr_mod_cr,
        [OP_MOD_RR] = 		&&instr_mod_rr,
        [OP_POW_RC] = 		&&instr_pow_rc,
        [OP_POW_CR] = 		&&instr_pow_cr,
        [OP_POW_RR] = 		&&instr_pow_rr,
        [OP_EEQ_RC] = 		&&instr_eeq_rc,
        [OP_EEQ_RR] = 		&&instr_eeq_rr,
        [OP_NEQ_RC] = 		&&instr_neq_rc,
        [OP_NEQ_RR] = 		&&instr_neq_rr,
        [OP_LT_RC] = 		&&instr_lt_rc,
        [OP_LT_RR] = 		&&instr_lt_rr,
        [OP_GT_RC] = 		&&instr_gt_rc,
        [OP_GT_RR] = 		&&instr_gt_rr,
        [OP_LTEQ_RC] = 		&&instr_lteq_rc,
        [OP_LTEQ_RR] = 		&&instr_lteq_rr,
        [OP_GTEQ_RC] = 		&&instr_gteq_rc,
        [OP_GTEQ_RR] = 		&&instr_gteq_rr,
        [OP_BAND_RC] = 		&&instr_band_rc,
        [OP_BAND_RR] = 		&&instr_band_rr,
        [OP_BOR_RC] = 		&&instr_bor_rc,
        [OP_BOR_RR] = 		&&instr_bor_rr,
        [OP_BXOR_RC] = 		&&instr_bxor_rc,
        [OP_BXOR_RR] = 		&&instr_bxor_rr,
        [OP_BNOT] = 		&&instr_bnot,
        [OP_NOT] = 		    &&instr_not,
        [OP_JUMP] = 		&&instr_jump,
        [OP_JFALSE] = 		&&instr_jfalse,
        [OP_NFOR_INIT] = 	&&instr_nfor_init,
        [OP_NFOR_LOOP] = 	&&instr_nfor_loop,
        [OP_GFOR_INIT] = 	&&instr_gfor_init,
        [OP_GFOR_LOOP] = 	&&instr_gfor_loop,
        [OP_LOAD_CR] = 		&&instr_load_cr,
        [OP_LOAD_RR] = 		&&instr_load_rr,
        [OP_LOAD_UP] = 		&&instr_load_up,
        [OP_LOAD_GLOBAL] = 	&&instr_load_global,
        [OP_LOAD_FIELD] = 	&&instr_load_field,
        [OP_CALL] = 		&&instr_call,
        [OP_CALL_METHOD] = 	&&instr_call_method,
        [OP_RETURN] = 		&&instr_return,
        [OP_NEWTABLE] = 	&&instr_newtable,
        [OP_ADDTABLE] = 	&&instr_addtable,
        [OP_GETTABLE] = 	&&instr_gettable,
        [OP_SETTABLE] = 	&&instr_settable,
        [OP_SETFIELD] = 	&&instr_setfield,
        [OP_NEWOBJECT] = 	&&instr_newobject,
        [OP_HALT] = 		&&instr_halt
    };

    TFunc* tfunc = AS_FUNC(func);
    StackFrame frame = {
        .func = tfunc,
        .ip = tfunc->bytescriptFunction.bytecode,
        .prev = &context.frame
    };

    context.frame = frame;
    uint8_t arg1; uint8_t arg2; uint8_t arg3;

    frame.ip -= 4;
    NEXT_INSTR;

    instr_add_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num + CONSTANT(arg3).num );
        NEXT_INSTR;
        
    instr_add_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num + context.sp[arg3].num );
        NEXT_INSTR;

    instr_sub_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num - CONSTANT(arg3).num );
        NEXT_INSTR;

    instr_sub_cr:
        context.sp[arg1] = NUM_AS_TVALUE( CONSTANT(arg2).num - context.sp[arg3].num );
        NEXT_INSTR;

    instr_sub_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num - context.sp[arg3].num );
        NEXT_INSTR;

    instr_mul_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num * CONSTANT(arg3).num );
        NEXT_INSTR;

    instr_mul_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num * context.sp[arg3].num );
        NEXT_INSTR;

    instr_div_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num / CONSTANT(arg3).num );
        NEXT_INSTR;

    instr_div_cr:
        context.sp[arg1] = NUM_AS_TVALUE( CONSTANT(arg2).num / context.sp[arg3].num );
        NEXT_INSTR;

    instr_div_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num / context.sp[arg3].num );
        NEXT_INSTR;

    instr_idiv_rc:
        context.sp[arg1] = NUM_AS_TVALUE( floor(context.sp[arg2].num / CONSTANT(arg3).num) );
        NEXT_INSTR;

    instr_idiv_cr:
        context.sp[arg1] = NUM_AS_TVALUE( floor(CONSTANT(arg2).num / context.sp[arg3].num) );
        NEXT_INSTR;

    instr_idiv_rr:
        context.sp[arg1] = NUM_AS_TVALUE( floor(context.sp[arg2].num / context.sp[arg3].num) );
        NEXT_INSTR;

    instr_mod_rc:
        context.sp[arg1] = NUM_AS_TVALUE( (int64_t)context.sp[arg2].num % (int64_t)CONSTANT(arg3).num );
        NEXT_INSTR;

    instr_mod_cr:
        context.sp[arg1] = NUM_AS_TVALUE( (int64_t)CONSTANT(arg2).num % (int64_t)context.sp[arg3].num );
        NEXT_INSTR;

    instr_mod_rr:
        context.sp[arg1] = NUM_AS_TVALUE( (int64_t)context.sp[arg2].num % (int64_t)context.sp[arg3].num );
        NEXT_INSTR;

    instr_pow_rc:
        context.sp[arg1] = NUM_AS_TVALUE( pow(context.sp[arg2].num, CONSTANT(arg3).num) );
        NEXT_INSTR;

    instr_pow_cr:
        context.sp[arg1] = NUM_AS_TVALUE( pow(CONSTANT(arg2).num, context.sp[arg3].num) );
        NEXT_INSTR;

    instr_pow_rr:
        context.sp[arg1] = NUM_AS_TVALUE( pow(context.sp[arg2].num, context.sp[arg3].num) );
        NEXT_INSTR;

    instr_eeq_rc:
        context.sp[arg1] = NUM_AS_TVALUE( EQUAL(context.sp[arg2], CONSTANT(arg3)) );
        NEXT_INSTR;

    instr_eeq_rr:
        context.sp[arg1] = NUM_AS_TVALUE( EQUAL(CONSTANT(arg2), context.sp[arg3]) );
        NEXT_INSTR;

    instr_neq_rc:
        context.sp[arg1] = NUM_AS_TVALUE( !EQUAL(context.sp[arg2], CONSTANT(arg3)) );
        NEXT_INSTR;

    instr_neq_rr:
        context.sp[arg1] = NUM_AS_TVALUE( !EQUAL(CONSTANT(arg2), context.sp[arg3]) );
        NEXT_INSTR;

    instr_lt_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num < CONSTANT(arg3).num );
        NEXT_INSTR;
        
    instr_lt_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num < context.sp[arg3].num );
        NEXT_INSTR;

    instr_gt_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num > CONSTANT(arg3).num );
        NEXT_INSTR;
        
    instr_gt_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num > context.sp[arg3].num );
        NEXT_INSTR;

    instr_lteq_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num <= CONSTANT(arg3).num );
        NEXT_INSTR;
        
    instr_lteq_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num <= context.sp[arg3].num );
        NEXT_INSTR;

    instr_gteq_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num >= CONSTANT(arg3).num );
        NEXT_INSTR;
        
    instr_gteq_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].num >= context.sp[arg3].num );
        NEXT_INSTR;

    instr_band_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan & CONSTANT(arg3).nan );
        NEXT_INSTR;
        
    instr_band_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan & context.sp[arg3].nan );
        NEXT_INSTR;

    instr_bor_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan | CONSTANT(arg3).nan );
        NEXT_INSTR;
        
    instr_bor_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan | context.sp[arg3].nan );
        NEXT_INSTR;

    instr_bxor_rc:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan ^ CONSTANT(arg3).nan );
        NEXT_INSTR;
        
    instr_bxor_rr:
        context.sp[arg1] = NUM_AS_TVALUE( context.sp[arg2].nan ^ context.sp[arg3].nan );
        NEXT_INSTR;

    instr_bnot:
        context.sp[arg1] = NUM_AS_TVALUE( ~context.sp[arg2].nan );
        NEXT_INSTR;

    instr_not:
        context.sp[arg1] = NUM_AS_TVALUE( FALSY(context.sp[arg2]) );
        NEXT_INSTR;
        
    instr_jump:
        frame.ip += off24_to_int32(arg1, arg2, arg3) * 4;
        NEXT_INSTR;
        
    instr_jfalse:
        if (TRUTHY(context.sp[arg1])) frame.ip += off16_to_int16(arg2, arg3) * 4;
        NEXT_INSTR;
        
    instr_nfor_init:
        context.sp[arg1].num -= context.sp[arg1 + 2].num;
        frame.ip += off16_to_int16(arg2, arg3) * 4;
        NEXT_INSTR;

    instr_nfor_loop: {
        int64_t step = context.sp[arg1 + 2].num;
        context.sp[arg1].num += step;

        if (step >= 0) {
            if (context.sp[arg1].num < context.sp[arg1 + 1].num) frame.ip += off16_to_int16(arg2, arg3) * 4;
        } else {
            if (context.sp[arg1].num > context.sp[arg1 + 1].num) frame.ip += off16_to_int16(arg2, arg3) * 4;
        }

        NEXT_INSTR;
    }

    instr_gfor_init:
        context.sp[arg1].num = -1;
        frame.ip += off16_to_int16(arg2, arg3) * 4;

    instr_gfor_loop: {
        uint64_t idx = ++context.sp[arg1].num;
        TTable* tbl = AS_TABLE(context.sp[arg1 + 2]);

        if (idx < tbl->array.len) context.sp[arg1 + 1] = tbl->array.array[idx];
        else frame.ip += off16_to_int16(arg2, arg3) * 4;

        NEXT_INSTR;
    }

    instr_load_cr:
        context.sp[arg2] = CONSTANT(arg1);
        NEXT_INSTR;

    instr_load_rr:
        context.sp[arg2] = context.sp[arg1];
        NEXT_INSTR;

    instr_load_up:
        NEXT_INSTR;

    instr_load_global:
        context.sp[arg3] = AS_TABLE(globals)->array.array[off16_to_int16(arg1, arg2)];
        NEXT_INSTR;

    instr_load_field:
        context.sp[arg3] = AS_OBJECT(context.sp[arg1])->fields[arg2];
        NEXT_INSTR;

    instr_call: {
        TValue caller = context.sp[arg1];
        TFunc* caller_func = AS_FUNC(caller);

        if (TYPE(caller) != FUNC) call_error(new_type_error, "Tried calling a non-callable value");
        if (caller_func->isCfunc) {
            context.sp[arg3] = caller_func->cfunc(context.sp + arg1 + 1, arg2);

        } else {
            StackFrame caller_frame = {
                .func = caller_func,
                .ip = caller_func->bytescriptFunction.bytecode,
                .prev = &context.frame
            };

            context.frame = caller_frame;
            context.frame.ip -= 4;
            context.sp[arg3] = (context.sp += arg1 + 1, run_function(caller, context));

            context.sp -= arg1 - 1;
            context.frame = *caller_frame.prev;
        }

        NEXT_INSTR;
    }

    instr_call_method:
        NEXT_INSTR;

    instr_return:
        return to_return_value(context.sp + arg1, arg2);

    instr_newtable:
        context.sp[arg3] = new_table(context.sp + arg1, arg2);
        NEXT_INSTR;

    instr_addtable:
        NEXT_INSTR;

    instr_gettable: {
        TValue  ttarget = context.sp[arg1];
        TTable* target = AS_TABLE(ttarget);
        TValue idx = context.sp[arg2];

        TValue value = IS_NUM(idx) ? target->array.array[(int64_t)context.sp[arg2].num] : NIL;
        value = IS_NIL(value) ? get_value_from_hashtable(ttarget, idx) : value;

        context.sp[arg3] = value;

        NEXT_INSTR;
    }

    instr_settable: {
        TValue ttarget = context.sp[arg1];
        TTable* target = AS_TABLE(ttarget);
        TValue idx = context.sp[arg3];
        TValue value = context.sp[arg2];

        if (!IS_TABLE(ttarget)) call_error(new_type_error, "Tried Indexing a non-table value");

        if (IS_NUM(idx) && (size_t)idx.num == target->array.len) insert_table(context.sp + arg1, 2);
        else add_value_to_hashtable(ttarget, idx, value);

        NEXT_INSTR;
    }

    instr_setfield:
        AS_OBJECT(context.sp[arg1])->fields[(int64_t)context.sp[arg2].num] = context.sp[arg3];
        NEXT_INSTR;

    instr_newobject:
        context.sp[arg3] = new_object(context.sp[arg1], NULL, arg2);
        NEXT_INSTR;

    instr_halt:

    return NIL;
}