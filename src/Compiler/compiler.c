#include <stdlib.h>
#include <math.h>

#include "Compiler/compiler.h"
#include "labeler.h"

#define false 0
#define true 1

CompileFunction compiler_functions[] = {
    [NT_STATEMENTS]     = compile_statements,
    [NT_RETURN]         = compile_return,
    [NT_BREAK]          = compile_break,
    [NT_CONTINUE]       = compile_continue,
    [NT_VAR_DECLARE]    = compile_var_declare,
    [NT_ASSIGNEMENT]    = compile_assignement,
    [NT_BIN_OP]         = compile_bin_op,
    [NT_UNI_OP]         = compile_uni_op,
    [NT_CALL]           = compile_call,
    [NT_VAR_ACESS]      = compile_var_access,
    [NT_NUMBER]         = compile_number,
    [NT_STRING]         = compile_string,
    [NT_ALLOCATION]     = compile_allocation,
    [NT_IF]             = compile_if_statement,
    [NT_NFOR]           = compile_nfor_loop,
    [NT_GFOR]           = compile_gfor_loop,
    [NT_WHILE]          = compile_while_loop,
    [NT_REPEAT]         = compile_repeat_loop,
    [NT_IMPORT]         = compile_import,
    [NT_FUNCDEF]        = compile_func_def
};


int compile(Node* node, Scope* context) { return compiler_functions[node->nt](node, context); }

int compile_statements(Node* node, Scope* context) {
    node->cc = false;
    int res = 0;
    
    for (int i = 0; i < node->node.StatementsNode.len; i++) {
        Node* compile_node = node->node.StatementsNode.nodes[i];
        res = compile(compile_node, context);
        free(compile_node);
    }

    return res;
}

int compile_return(Node* node, Scope* context) {
    if (context->parent == NULL) {
        emit(OP_HALT, context);
        emit(0x00, context);
        emit(0x00, context);
        emit(0x00, context);
    }

    node->cc = false;
    int r0 = free_register(register_top(context), context);

    if (node->node.ReturnNode.len == 1) {
        Node* nd = node->node.ReturnNode.nodes[0];
        int return_v = compile(nd, context);

        if (nd->cc) r0 = register_constant(save_cached_constant(return_v, context), context, -1); else r0 = register_specific(return_v, context);
        free(nd);

    } else if (node->node.ReturnNode.len > 1) {
        for (int i = 0; i < node->node.ReturnNode.len; i++) register_top(context);
        for (int i = 0; i < node->node.ReturnNode.len; i++) {
            Node* nd = node->node.ReturnNode.nodes[i];
            int return_v = compile(nd, context);

            if (nd->cc) register_constant(save_cached_constant(return_v, context), context, r0 + i); else register_register(return_v, context, r0 + i);
            free(nd);
        }
    }

    emit(OP_RETURN, context);
    emit(free_multiple(r0, node->node.ReturnNode.len, context), context);
    emit(node->node.ReturnNode.len, context);
    emit(0x00, context);
    return r0;
}

int compile_break(Node* node, Scope* context) {
    struct uint8LinkedList current = context->breaks;
    while (current.next != NULL) current = *current.next;

    current.ptr = CURRENT_INSTR;
    current.next = malloc(sizeof(struct uint8LinkedList));
    if (!current.next) FAILED_ALLOCATION_ERROR("Failed allocation while compiling");

    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    return 0;
}

int compile_continue(Node* node, Scope* context) {
    struct uint8LinkedList current = context->breaks;
    while (current.next != NULL) current = *current.next;

    current.ptr = CURRENT_INSTR;
    current.next = malloc(sizeof(struct uint8LinkedList));
    if (!current.next) FAILED_ALLOCATION_ERROR("Failed allocation while compiling");

    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    return 0;
}

int compile_var_declare(Node* node, Scope* context) {
    int r = -1;
    int value = compile(node->node.VarDeclareNode.value, context);

    if (node->node.VarDeclareNode.value->cc) r = register_constant(save_cached_constant(value, context), context, -1); else r = register_register(value, context, -1);
    append_symbol(context, node->node.VarDeclareNode.var_name, r);

    free(node->node.VarDeclareNode.value);
    return r;
}

int compile_assignement(Node* node, Scope* context) {
    node->cc = false;
    int lvalue = compile(node->node.AssignementNode.lvalue, context);
    int value = -1;

    switch (node->node.AssignementNode.lvalue->nt) {
        case NT_VAR_ACESS: {
            int analysis_res = assignement_analysis(node);
            if (analysis_res) free_register(lvalue, context);

            value = compile(node->node.AssignementNode.value, context);
            if (node->node.AssignementNode.value->cc) value = register_constant(save_cached_constant(value, context), context, lvalue);

            break;
        }

        default : INCORRECT_LVALUE("cannot assign value to rvalue");
    }

    free(node->node.AssignementNode.lvalue);
    free(node->node.AssignementNode.value);
    return register_register(value, context, lvalue);
}

int compile_bin_op(Node* node, Scope* context) {
    int left = compile(node->node.BinOpNode.left, context);
    bool leftCC = node->node.BinOpNode.left->cc;

    int free_left = leftCC ? 0 : register_try_free(left, context) + 1;

    int right = compile(node->node.BinOpNode.right, context);
    bool rightCC = node->node.BinOpNode.right->cc;

    int op = node->node.BinOpNode.op.tt;

    if (free_left) free_register(left, context);
    if (leftCC && rightCC) {
        TValue folded = operation_fold(left, op, right, context);
        delete_constant(left, context);
        delete_constant(right, context);
        
        node->cc = true;
        return cache_constant(folded, context);
    }

    bool CC = leftCC || rightCC;
    if (leftCC) left = save_cached_constant(left, context); if (rightCC) right = save_cached_constant(right, context);
    if (leftCC) { int temp = left; left = right; right = temp; }

    switch (op) {
        case TT_PLUS:   emit(CC ? OP_ADD_RC  : OP_ADD_RR, context); break;
        case TT_MUL:    emit(CC ? OP_MUL_RC  : OP_MUL_RR, context); break;
        case TT_AND:    emit(CC ? OP_AND_RC  : OP_AND_RR, context); break;
        case TT_OR:     emit(CC ? OP_OR_RC   : OP_OR_RR, context); break;
        case TT_BAND:   emit(CC ? OP_BAND_RC : OP_BAND_RR, context); break;
        case TT_BOR:    emit(CC ? OP_BOR_RC  : OP_BOR_RR, context); break;
        case TT_BXOR:   emit(CC ? OP_BXOR_RC : OP_BXOR_RR, context); break;
        case TT_EEQ:    emit(CC ? OP_EEQ_RC  : OP_EEQ_RR, context); break;
        case TT_GT:     emit(leftCC ? OP_LT_RC : rightCC ? OP_GT_RC : OP_GT_RR, context); break;
        case TT_LT:     emit(leftCC ? OP_GT_RC : rightCC ? OP_LT_RC : OP_GT_RR, context); break;
        case TT_GTEQ:   emit(leftCC ? OP_LTEQ_RC : rightCC ? OP_GTEQ_RC : OP_GTEQ_RR, context); break;
        case TT_LTEQ:   emit(leftCC ? OP_GTEQ_RC : rightCC ? OP_LTEQ_RC : OP_LTEQ_RR, context); break;

        default :
            if (leftCC) { int temp = left; left = right; right = temp; }

            switch (op) {
                case TT_MINUS:  emit(leftCC ? OP_SUB_CR    : rightCC ? OP_SUB_RC     : OP_SUB_RR, context); break;
                case TT_DIV:    emit(leftCC ? OP_DIV_CR    : rightCC ? OP_DIV_RC     : OP_DIV_RR, context); break;
                case TT_IDIV:   emit(leftCC ? OP_IDIV_CR   : rightCC ? OP_IDIV_RC    : OP_IDIV_RR, context); break;
                case TT_MOD:    emit(leftCC ? OP_MOD_CR    : rightCC ? OP_MOD_RC     : OP_MOD_RR, context); break;
                case TT_POW:    emit(leftCC ? OP_POW_CR    : rightCC ? OP_POW_RC     : OP_POW_RR, context); break;
                case TT_CONCAT: emit(leftCC ? OP_CONCAT_CR : rightCC ? OP_CONCAT_RC  : OP_CONCAT_RR, context); break;
            }
    }

    int r = free_register(register_first(context), context);
    emit(r, context);
    emit(left, context);
    emit(right, context);

    node->cc = false;

    free(node->node.BinOpNode.left);
    free(node->node.BinOpNode.right);
    return r;
}

int compile_uni_op(Node* node, Scope* context) {
    int r = 0;
    int target = compile(node->node.UniOpNode.target, context);
    node->cc = node->node.UniOpNode.target->cc;

    switch (node->node.UniOpNode.op.tt) {
        case TT_PLUS: return target;
        case TT_MINUS: {
            if (node->cc) {
                TValue v = get_constant(target, context);
                if (!IS_NUM(v)) TYPE_ERROR("cannot proceed to the unary operation '-' on a type other than number");

                return cache_constant(NUM_AS_TVALUE(-AS_NUM(v)), context);
            }

            r = free_register(register_first(context), context);
            emit(OP_MUL_RC, context);
            emit(r, context);
            emit(target, context);
            emit(save_cached_constant(cache_constant(NUM_AS_TVALUE(-1), context), context), context);
        }

        case TT_NOT: {
            if (node->cc) return cache_constant(NUM_AS_TVALUE(!TRUTHY(get_constant(target, context))), context);

            int r = free_register(register_first(context), context);
            emit(OP_BNOT, context);
            emit(r, context);
            emit(target, context);
            emit(0x000, context);
        }
    }

    free(node->node.UniOpNode.target);
    return r;
}

int compile_call(Node* node, Scope* context) {
    node->cc = false;

    int target = compile(node->node.CallNode.target, context);
    int target_cc = node->node.CallNode.target->cc;
    int pre = target;

    if (target_cc) target = register_constant(target, context, register_top(context)); else target = register_specific_top(target, context);

    for (int i = 0; i < node->node.CallNode.args.len; i++) assign_to_reg(node->node.CallNode.args.nodes[i], register_top(context), context);
    free_multiple(!target_cc && pre == target ? target + 1 : target, node->node.CallNode.args.len, context);

    emit(OP_CALL, context);
    emit(target, context);
    emit(node->node.CallNode.args.len, context);
    return free_register(emit(register_first(context), context), context);
}

int compile_var_access(Node* node, Scope* context) {
    node->cc = false;
    TValue tidx = get_symbol_register(context, node->node.VarAcess.name);

    if (IS_NIL(tidx)) {
        TStr* name = AS_STRING(node->node.VarAcess.name);

        char details[snprintf(NULL, 0, "identifier '%s' does not exist", name->str) + 1];
        snprintf(details, sizeof(details), "identifier '%s' does not exist", name->str);
        UNKNOWN_IDENTIFIER(details);
    }

    return (int)(AS_NUM(tidx));
}

int compile_number(Node* node, Scope* context) {
    node->cc = true;
    return cache_constant(node->node.NumberNode.num, context);
}

int compile_string(Node* node, Scope* context) {
    node->cc = true;
    return cache_constant(node->node.StringNode.str, context);
}

int compile_allocation(Node* node, Scope* context) {
    //
}

int compile_if_statement(Node* node, Scope* context) {
    NodeArray cases = node->node.IfNode.cases;
    NodeArray stments = node->node.IfNode.statements;
    uint8_t* next = NULL;

    struct { uint8_t* ptr[cases.len]; size_t len; } ends;
    ends.len = 0;

    node->cc = false;
    for (int i = 0; i < cases.len; i++) {
        int condition = compile(cases.nodes[i], context);

        if (cases.nodes[i]->cc) {
            if (AS_NUM(get_constant(condition, context))) {
                node->cc = true;
                
                compile_in_scope(stments.nodes[i], context);

                break;

            } else continue;
        }

        emit(OP_JFALSE, context);
        emit(condition, context);
        emit(0x00, context);
        emit(0x00, context);
        next = CURRENT_INSTR;

        compile_in_scope(stments.nodes[i], context);

        emit(OP_JUMP, context);
        emit(0x00, context);
        emit(0x00, context);
        emit(0x00, context);
        ends.ptr[ends.len++] = CURRENT_INSTR;

        patch_single_jfalse_label(next, get_jump_lenght(next, context));
    }


    if (!node->cc) {
        if (node->node.IfNode.cases.len < node->node.IfNode.statements.len) {
            compile(stments.nodes[stments.len - 1], context);
            
        } else { context->bytecode.len -= 4; patch_single_jfalse_label(next, get_jump_lenght(next, context)); }

        patch_array_jump_label((struct ArrayLabel){ .ptr = ends.ptr, .len = ends.len }, CURRENT_INSTR);
    }

    return 0;
}

int compile_nfor_loop(Node* node, Scope* context) {
    node->cc = false;

    open_scope(context);

    Node* start_value_node = node->node.nForNode.values[0];
    Node* end_value_node = node->node.nForNode.values[1];
    Node* step_value_node = node->node.nForNode.values[2];

    int start_value = compile(node->node.nForNode.values[0], context);
    int r0 = start_value_node->cc ? register_constant(start_value, context, register_top(context)) : register_try_newtop(start_value, context);
    int r1 = register_top(context);
    int r2 = register_top(context);

    assign_to_reg(step_value_node, r2, context);
    assign_to_reg(end_value_node, r1, context);

    append_symbol(context, node->node.nForNode.identifier, r0);

    emit(OP_NFOR_INIT, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);
    uint8_t* init = CURRENT_INSTR;

    compile(node->node.nForNode.statements, context);
    patch_single_jfalse_label(init, get_jump_lenght(init, context));

    emit(OP_NFOR_LOOP, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);

    uint8_t* loop = CURRENT_INSTR;
    patch_single_jfalse_label(loop, get_jump_lenght_to(loop, context, init));

    close_scope(context);
}

int compile_gfor_loop(Node* node, Scope* context) {
    node->cc = false;

    open_scope(context);

    int target = compile(node->node.gForNode.target, context);

    int r0 = register_top(context);
    int r1 = register_top(context);
    int r2 = register_register(target, context, register_top(context));

    append_symbol(context, node->node.gForNode.identifiers[0], r0);
    if (!IS_NIL(node->node.gForNode.identifiers[1])) append_symbol(context, node->node.gForNode.identifiers[1], r0);

    emit(OP_GFOR_INIT, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);
    uint8_t* init = CURRENT_INSTR;

    compile(node->node.gForNode.statements, context);
    patch_single_jfalse_label(init, get_jump_lenght(init, context));

    emit(OP_GFOR_LOOP, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);

    uint8_t* loop = CURRENT_INSTR;
    patch_single_jfalse_label(loop, get_jump_lenght_to(loop, context, init));

    close_scope(context);
}

int compile_while_loop(Node* node, Scope* context) {
    node->cc = false;

    uint8_t* pre_condition = CURRENT_INSTR;
    int condition = compile(node->node.whileNode.condition, context);
    
    emit(OP_JFALSE, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);
    uint8_t* condition_label = CURRENT_INSTR;

    compile_in_scope(node->node.whileNode.statements, context);

    emit(OP_JUMP, context);
    emit(condition, context);
    emit(0x00, context);
    emit(0x00, context);

    patch_single_jump_label(CURRENT_INSTR, get_jump_lenght_to(CURRENT_INSTR, context, pre_condition));
    patch_single_jfalse_label(condition_label, get_jump_lenght(condition_label, context));
}

int compile_repeat_loop(Node* node, Scope* context) {
    node->cc = false;

    uint8_t* pre_statement = CURRENT_INSTR;
    compile_in_scope(node->node.whileNode.statements, context);
    int condition = compile(node->node.whileNode.condition, context);
    
    emit(OP_JFALSE, context);
    emit(condition, context);
    emit(0x00, context);
    emit(0x00, context);
    uint8_t* condition_label = CURRENT_INSTR;

    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    patch_single_jump_label(CURRENT_INSTR, get_jump_lenght_to(CURRENT_INSTR, context, pre_statement));
    patch_single_jfalse_label(condition_label, get_jump_lenght(condition_label, context));
}

int compile_import(Node* node, Scope* context) {
    node->cc = true;


}

int compile_func_def(Node* node, Scope* context) {
    node->cc = true;

    Scope* func_context = new_scope();
    func_context->parent = context;

    struct TValueLinkedList* arg = node->node.funcDefNode.args.next;
    while (arg) {
        int r = register_first(func_context);
        append_symbol(func_context, arg->value, r);

        struct TValueLinkedList* new = arg->next;
        free(arg); arg = new;
    }

    compile(node->node.funcDefNode.statements, func_context);
    TValue func = close_context(func_context);

    int idx = save_cached_constant(cache_constant(func, context), context);
    if (!IS_NIL(node->node.funcDefNode.name)) {
        idx = register_constant(idx, context, -1);
        append_symbol(context, node->node.funcDefNode.name, idx);
        node->cc = false;
    }

    return idx;
}



Scope* new_scope() {
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;
    scope->symbol_stack = AS_TABLE(new_table(NULL, 0));
    open_scope(scope);

    size_t CONST_POOL_SIZE = sizeof(scope->const_pool.const_pool) / TVALUESIZE;
    for (int i = 0; i < CONST_POOL_SIZE; i++) { scope->const_pool.const_pool[i] = NIL; }
    scope->const_pool.len = 0;
    scope->saved_const.len = 0;

    scope->free_regs.len = 0;
    scope->reg_ptr = 0;
    scope->breaks.ptr = NULL;
    scope->breaks.next = NULL;
    scope->continues.ptr = NULL;
    scope->continues.next = NULL;

    scope->bytecode.bytes = malloc(64);
    scope->bytecode.len = 0;
    scope->bytecode.cap = 64;
    scope->bytecode.bytes[0] = OP_HALT;

    return scope;
}

int emit(uint8_t instr, Scope* context) {
    context->bytecode.bytes[context->bytecode.len++] = instr;
    if (context->bytecode.len == context->bytecode.cap) {
        uint8_t* new = realloc(context->bytecode.bytes, (context->bytecode.cap *= 2));
        if (new == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while emiting instruction");

        context->bytecode.bytes = new;
    }

    return instr;
}

static int register_first(Scope* context) {
    if (context->free_regs.len > 0) return context->free_regs.free_regs[context->free_regs.len - 1];
    return register_top(context);
}

static int free_register(uint8_t idx, Scope* context) {
    if (idx != context->reg_ptr - 1) {
        for (int i = 0; i < context->free_regs.len; i++) {
            if (context->free_regs.free_regs[i] == idx) return idx;
        }

        return (context->free_regs.free_regs[context->free_regs.len++] = idx);
    }

    return --context->reg_ptr;
}

static int register_constant(uint8_t const_i, Scope* context, int16_t r_) {
    uint8_t r = r_ + 1 ? r_ : register_first(context);
    emit(OP_LOAD_CR, context);
    emit(const_i, context);
    emit(register_specific(r, context), context);
    emit(0x00, context);

    return r;
}

static int register_register(uint8_t r, Scope* context, int16_t r_) {
    uint8_t r2 = r_ + 1 ? r_ : register_first(context);
    if (r2 != r) {
        emit(OP_LOAD_RR, context);
        emit(r, context);
        emit(register_specific(r2, context), context);
        emit(0x00, context);

    } else register_specific(r2, context);

    return r2;
}

static int register_try_free(uint8_t r, Scope* context) {
    for (int i = 0; i < context->free_regs.len; i++) {
        if (r == context->free_regs.free_regs[i]) {
            for (int i2 = i; i2 < context->free_regs.len - 1; i2++) {
                context->free_regs.free_regs[i2] = context->free_regs.free_regs[i2 + 1];
            }

            context->free_regs.len--;
            return r;
        }
    }

    if (r == context->reg_ptr) return context->reg_ptr++;
    return -1;
}

static int register_try_newtop(uint8_t r, Scope* context) {
    if (r == context->reg_ptr) return context->reg_ptr++;
    else return register_register(r, context, register_top(context));
}

static int register_specific_top(uint8_t r, Scope* context) {
    if (r == context->reg_ptr - 1) return r;
    else if (r == context->reg_ptr) return context->reg_ptr++;
    else return register_register(r, context, register_top(context));
}

static int free_multiple(uint8_t r0, uint8_t len, Scope* context) {
    for (int i = len - 1; i >= r0; i--) {
        free_register(i, context);
    }

    return r0;
}

static int get_constant_idx(TValue const_v, Scope* context) {
    for (int i = 0; i < context->const_pool.len; i++) {
        if (AS_TVALUE(context->const_pool.const_pool[i]) == AS_TVALUE(const_v)) return i;
    }

    return -1;
}

static int cache_constant(TValue const_v, Scope* context) {
    int idx = get_constant_idx(const_v, context);
    if (idx != -1) return idx;

    if (context->const_pool.len == 0xff) STACK_OVERFLOW("Fatal Error : Constant overflow, exceeded constants limit");
    context->const_pool.const_pool[context->const_pool.len++] = const_v;

    return context->const_pool.len - 1;
}

static int is_saved(TValue const_v, Scope* context) { 
    for (int i = 0; i < context->saved_const.len; i++) {
        if (AS_TVALUE(context->saved_const.saved_consts[i]) == AS_TVALUE(const_v)) return true;
    }

    return false;
}

static inline int register_top(Scope* context) {
    if (context->reg_ptr == 0xff) STACK_OVERFLOW("Fatal Error : Register overflow, exceeded register limit");
    return context->reg_ptr++;
}

static inline int register_specific(uint8_t r, Scope* context) {
    register_try_free(r, context);
    return r;
}

static inline int register_try(uint8_t r, Scope* context) {
    int new_r = register_try_free(r, context);
    if (!(new_r + 1)) register_first(context); else return r;
}

static inline int save_cached_constant(int idx, Scope* context) { save_constant(get_constant(idx, context), context); return idx; }

static inline int correct_constant_idx(int idx, Scope* context) { return idx < context->const_pool.len ? 1 : 0; }

static inline void sure_delete_constant(int idx, Scope* context) {
    shift_tvalue_array(context->const_pool.const_pool, context->const_pool.len, idx);
    context->const_pool.len--;
}

static void delete_constant(int idx, Scope* context) {
    TValue const_v = get_constant(idx, context);
    if (is_saved(const_v, context) || !correct_constant_idx(idx, context)) return;

    sure_delete_constant(idx, context);
}

static void unsave_cached_constant(int idx, Scope* context) {
    TValue target = get_constant(idx, context);

    for (int i = 0; i < context->saved_const.len; i++) {
        TValue cst = context->saved_const.saved_consts[i];

        if (AS_TVALUE(cst) == AS_TVALUE(target)) {
            shift_tvalue_array(context->saved_const.saved_consts, 256, i);
            context->saved_const.len--;
            break;
        }
    }

    sure_delete_constant(idx, context);
}

static inline TValue save_constant(TValue const_v, Scope* context) { context->saved_const.saved_consts[context->saved_const.len++] = const_v; return const_v; }

static inline TValue get_constant(int idx, Scope* context) { return context->const_pool.const_pool[idx]; }

static TValue operation_fold(int idx1, TokenType op, int idx2, Scope* context) {
    TValue v1 = get_constant(idx1, context);
    TValue v2 = get_constant(idx2, context);
    if (op == TT_EEQ) return NUM_AS_TVALUE(AS_TVALUE(v1) == AS_TVALUE(v2));

    switch (TYPE(v1)) {
        case STRING : {
            if (TYPE(v2) != STRING) TYPE_ERROR("second value to this operation must be a string");

            switch (op) {
                case TT_CONCAT: return concat_string((TValue[2]){ v1, v2 }, 2);
            }
        }

        default : {
            if (!IS_NUM(v2)) TYPE_ERROR("second value to this operation must be a number");

            switch (op) {
                case TT_PLUS:   return NUM_AS_TVALUE(AS_NUM(v1) + AS_NUM(v2));
                case TT_MINUS:  return NUM_AS_TVALUE(AS_NUM(v1) - AS_NUM(v2));
                case TT_MUL:    return NUM_AS_TVALUE(AS_NUM(v1) * AS_NUM(v2));
                case TT_DIV:    return NUM_AS_TVALUE(AS_NUM(v1) / AS_NUM(v2));
                case TT_IDIV:   return NUM_AS_TVALUE(floor(AS_NUM(v1) / AS_NUM(v2)));
                case TT_MOD:    return NUM_AS_TVALUE(fmod(AS_NUM(v1), AS_NUM(v2)));
                case TT_AND:    return NUM_AS_TVALUE(TRUTHY(v1) && TRUTHY(v2));
                case TT_OR:     return NUM_AS_TVALUE(TRUTHY(v1) || TRUTHY(v2));
                case TT_BAND:   return NUM_AS_TVALUE(AS_TVALUE(v1) & AS_TVALUE(v2));
                case TT_BOR:    return NUM_AS_TVALUE(AS_TVALUE(v1) | AS_TVALUE(v2));
                case TT_BXOR:   return NUM_AS_TVALUE(AS_TVALUE(v1) ^ AS_TVALUE(v2));
                case TT_GT:     return NUM_AS_TVALUE(AS_NUM(v1) > AS_NUM(v2));
                case TT_LT:     return NUM_AS_TVALUE(AS_NUM(v1) < AS_NUM(v2));
                case TT_GTEQ:   return NUM_AS_TVALUE(AS_NUM(v1) >= AS_NUM(v2));
                case TT_LTEQ:   return NUM_AS_TVALUE(AS_NUM(v1) <= AS_NUM(v2));
                case TT_POW:    return NUM_AS_TVALUE(pow(AS_NUM(v1), AS_NUM(v2)));
            }
        }
    }
}

static inline int assignement_analysis(Node* node) {
    int depth = 0;
    int target_count = 0;
    return full_assignement_analysis(node->node.AssignementNode.value, node->node.AssignementNode.lvalue, &depth, &target_count);
}

static inline int assign_to_reg(Node* node, uint8_t r, Scope* context) {
    int depth = 0; int target_count = 0;
    int analysis = full_assignement_analysis(node, (Node[1]){ (Node){ .nt = NT_PLACEHOLDER } }, &depth, &target_count);
    if (analysis) free_register(r, context);

    int end_value = compile(node, context);
    if (node->cc) register_constant(end_value, context, r); else register_register(end_value, context, r);

    return r;
}

static int full_assignement_analysis(Node* node, Node* target, int* depth, int* target_count) {
    #define ADD_DEPTH ( ((*depth)++), depth )

    if (node_equal(target, node)) target_count++;
    if (*depth > 1 && *target_count > 0) return 0;

    switch (node->nt) {
        case NT_VAR_DECLARE: return 0;
        case NT_UNI_OP: return full_assignement_analysis(node->node.UniOpNode.target, target, ADD_DEPTH, target_count);

        case NT_ASSIGNEMENT: return 
            full_assignement_analysis(node->node.AssignementNode.lvalue, target, ADD_DEPTH, target_count) &&
            full_assignement_analysis(node->node.AssignementNode.value, target, depth, target_count);

        case NT_BIN_OP: return full_assignement_analysis(node->node.BinOpNode.left, target, depth, target_count) &&
                               full_assignement_analysis(node->node.BinOpNode.right, target, ADD_DEPTH, target_count);
            
        case NT_CALL:
            int endv = 1;
            *depth += node->node.CallNode.args.len;
            for (int i = 0; i < node->node.CallNode.args.len; i++) endv = endv && full_assignement_analysis(node->node.CallNode.args.nodes[i], target, depth, target_count);
            return endv;

        default: return 1;
    }
}


static void open_scope(Scope* context) {
    append_to_table(TBL_TO_TVALUE(context->symbol_stack), new_table(NULL, 0));
    update_symbols(context);
}

static void close_scope(Scope* context) {
    TValue last_scope = get_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1));
    TValue registers = hashed_values_table((TValue[1]){ last_scope }, 1);
    TTable* registers_tbl = AS_TABLE(registers);

    for (int i = 0; i < registers_tbl->array.len; i++) { free_register((uint8_t)AS_NUM(registers_tbl->array.array[i]), context); }
    
    free_table(&last_scope, 1);
    free_table(&registers, 1);
    remove_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1));
    update_symbols(context);
}

static inline int compile_in_scope(Node* node, Scope* context) {
    open_scope(context);
    int res = compile(node, context);
    close_scope(context);

    return res;
}

static inline void update_symbols(Scope* context) {
    if (!context->symbol_stack->array.len) return;
    context->symbols = AS_TABLE(get_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1)));
}

static TValue append_symbol(Scope* context, TValue name, int r) { add_value_to_hashtable(TBL_TO_TVALUE(context->symbols), name, NUM_AS_TVALUE(r)); }

static TValue get_symbol_register(Scope* context, TValue name) {
    for (int i = context->symbol_stack->array.len - 1; i >= 0; i--) {
        TValue symbol_table = context->symbol_stack->array.array[i];
        TValue tidx = get_value_from_hashtable(symbol_table, name);
        if (!IS_NIL(tidx)) return tidx;
    }

    return NIL;
}

TValue close_context(Scope* context) {
    emit(OP_HALT, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    uint8_t* bytecode = realloc(context->bytecode.bytes, context->bytecode.len);
    if (!bytecode) FAILED_ALLOCATION_ERROR("Failed allocation while closing compile context");

    TValue func = new_func(bytecode, context->const_pool.const_pool, context->const_pool.len, NULL);

    close_scope(context);
    free_table((TValue[1]){ TBL_TO_TVALUE(context->symbol_stack) }, 1);
    return func;
}
