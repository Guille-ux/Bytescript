#include <stdlib.h>
#include <math.h>
#include "Compiler/compiler.h"

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
    [NT_STRING]         = compile_string
};


int compile(Node* node, Scope* context) { printf("here we go again %d\n", node->nt); return compiler_functions[node->nt](node, context); }

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

    } else if (node->node.ReturnNode.len > 1) {
        for (int i = 0; i < node->node.ReturnNode.len; i++) register_top(context);
        for (int i = 0; i < node->node.ReturnNode.len; i++) {
            Node* nd = node->node.ReturnNode.nodes[i];
            int return_v = compile(nd, context);

            if (nd->cc) register_constant(save_cached_constant(return_v, context), context, r0 + i); else register_register(return_v, context, r0 + i);
        }
    }

    emit(OP_RETURN, context);
    emit(free_multiple(r0, node->node.ReturnNode.len, context), context);
    emit(node->node.ReturnNode.len, context);
    emit(0x00, context);
    return r0;
}

int compile_break(Node* node, Scope* context) {
    struct LinkedList current = context->breaks;
    while (current.next != NULL) current = *current.next;

    current.ptr = context->bytecode.bytes + context->bytecode.len;
    current.next = malloc(sizeof(struct LinkedList));
    if (!current.next) FAILED_ALLOCATION_ERROR("Failed allocation while compiling");

    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    return 0;
}

int compile_continue(Node* node, Scope* context) {
    struct LinkedList current = context->breaks;
    while (current.next != NULL) current = *current.next;

    current.ptr = context->bytecode.bytes + context->bytecode.len;
    current.next = malloc(sizeof(struct LinkedList));
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

    if (node->node.VarDeclareNode.value->cc) r = register_constant(save_cached_constant(value, context), context, -1); else r = register_try(value, context);

    add_value_to_hashtable(TBL_TO_TVALUE(context->symbols), node->node.VarDeclareNode.var_name, NUM_AS_TVALUE(r));
    return r;
}

int compile_assignement(Node* node, Scope* context) {
    node->cc = false;
    int lvalue = compile(node->node.AssignementNode.lvalue, context);
    int value = -1;

    switch (node->node.AssignementNode.lvalue->nt) {
        case NT_VAR_ACESS: {
            int analysis_res = assignement_analysis(node);
            printf("assignement %d\n", analysis_res);
            if (analysis_res) free_register(lvalue, context);

            value = compile(node->node.AssignementNode.value, context);
            if (node->node.AssignementNode.value->cc) value = register_constant(save_cached_constant(value, context), context, lvalue);

            break;
        }

        default : INCORRECT_LVALUE("cannot assign value to rvalue");
    }

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
        case TT_LT:     emit(leftCC ? OP_LT_RC : rightCC ? OP_GT_RC : OP_GT_RR, context); break;
        case TT_GTEQ:   emit(leftCC ? OP_LT_RC : rightCC ? OP_GT_RC : OP_GT_RR, context); break;
        case TT_LTEQ:   emit(leftCC ? OP_LT_RC : rightCC ? OP_GT_RC : OP_GT_RR, context); break;

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
    return r;
}

int compile_uni_op(Node* node, Scope* context) {
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

            int r = free_register(register_first(context), context);
            emit(OP_MUL_RC, context);
            emit(r, context);
            emit(target, context);
            emit(save_cached_constant(cache_constant(NUM_AS_TVALUE(-1), context), context), context);

            return r;
        }

        case TT_NOT: {
            if (node->cc) return cache_constant(NUM_AS_TVALUE(!TRUTHY(get_constant(target, context))), context);

            int r = free_register(register_first(context), context);
            emit(OP_BNOT, context);
            emit(r, context);
            emit(target, context);
            emit(0x000, context);

            return r;
        }


    }
}

int compile_call(Node* node, Scope* context) {
    //
}

int compile_var_access(Node* node, Scope* context) {
    node->cc = false;
    TValue tidx = get_value_from_hashtable(TBL_TO_TVALUE(context->symbols), node->node.VarAcess.name);

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


Scope* new_scope() {
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;
    scope->symbols = AS_TABLE(new_table((TValue[1]){ NIL }, 0));

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

static inline int emit(uint8_t instr, Scope* context) {
    context->bytecode.bytes[context->bytecode.len++] = instr;
    if (context->bytecode.len == context->bytecode.cap) {
        uint8_t* new = realloc(context->bytecode.bytes, (context->bytecode.cap *= 2));
        if (new == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while emiting instruction");

        context->bytecode.bytes = new;
    }

    return instr;
}

static inline int register_first(Scope* context) {
    if (context->free_regs.len > 0) return context->free_regs.free_regs[context->free_regs.len - 1];

    return register_top(context);
}

static inline int free_register(uint8_t idx, Scope* context) {
    if (idx != context->reg_ptr - 1) {
        for (int i = 0; i < context->free_regs.len; i++) {
            if (context->free_regs.free_regs[i] == idx) return idx;
        }

        return (context->free_regs.free_regs[context->free_regs.len++] = idx);
    }

    return --context->reg_ptr;
}

static inline int register_top(Scope* context) {
    if (context->reg_ptr == 0xff) STACK_OVERFLOW("Fatal Error : Register overflow, exceeded register limit");
    return context->reg_ptr++;
}

static inline int register_constant(uint8_t const_i, Scope* context, int16_t r_) {
    uint8_t r = r_ + 1 ? r_ : register_first(context);
    emit(OP_LOAD_CR, context);
    emit(const_i, context);
    emit(r, context);
    emit(0x00, context);

    return r;
}

static inline int register_register(uint8_t r, Scope* context, int16_t r_) {
    uint8_t r2 = r_ + 1 ? r_ : register_first(context);
    if (r2 != r) {
        emit(OP_LOAD_RR, context);
        emit(r, context);
        emit(register_specific(r2, context), context);
        emit(0x00, context);

    } else register_specific(r2, context);

    return r2;
}

static inline int register_specific(uint8_t r, Scope* context) {
    register_try_free(r, context);
    return r;
}

static inline int register_try(uint8_t r, Scope* context) {
    int new_r = register_try_free(r, context);
    if (!(new_r + 1)) register_first(context); else return r;
}

static inline int register_try_free(uint8_t r, Scope* context) {
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

static inline int free_multiple(uint8_t r0, uint8_t len, Scope* context) {
    for (int i = len - 1; i >= r0; i++) {
        free_register(i, context);
    }

    return r0;
}

static inline int get_constant_idx(TValue const_v, Scope* context) {
    for (int i = 0; i < context->const_pool.len; i++) {
        if (AS_TVALUE(context->const_pool.const_pool[i]) == AS_TVALUE(const_v)) return i;
    }

    return -1;
}

static inline int cache_constant(TValue const_v, Scope* context) {
    int idx = get_constant_idx(const_v, context);
    if (idx != -1) return idx;

    if (context->const_pool.len == 0xff) STACK_OVERFLOW("Fatal Error : Constant overflow, exceeded constants limit");
    context->const_pool.const_pool[context->const_pool.len++] = const_v;

    return context->const_pool.len - 1;
}

static inline int save_cached_constant(int idx, Scope* context) { save_constant(get_constant(idx, context), context); return idx; }

static inline int is_saved(TValue const_v, Scope* context) { 
    for (int i = 0; i < context->saved_const.len; i++) {
        if (AS_TVALUE(context->saved_const.saved_consts[i]) == AS_TVALUE(const_v)) return true;
    }

    return false;
}

static inline void sure_delete_constant(int idx, Scope* context) {
    shift_tvalue_array(context->const_pool.const_pool, context->const_pool.len, idx);
    context->const_pool.len--;
}

static inline void delete_constant(int idx, Scope* context) {
    TValue const_v = get_constant(idx, context);
    if (is_saved(const_v, context)) return;

    sure_delete_constant(idx, context);
}

static inline void unsave_cached_constant(int idx, Scope* context) {
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
