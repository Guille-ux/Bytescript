#include <stdlib.h>
#include "Compiler/compiler.h"
#include "Parser/nodes.h"

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


int compile(Node* node, Scope* context) { return compiler_functions[node->nt](node, context); }

int compile_statements(Node* node, Scope* context) {
    node->cc = false;
    
    for (int i = 0; i < node->node.StatementsNode.len; i++) {
        Node** compile_node = node->node.StatementsNode.nodes[i];
        compile(compile_node, context);
        free(*compile_node);
    }
}

int compile_return(Node* node, Scope* context) {
    if (context->parent == NULL) {
        emit(T_EOF, context);
        emit(0x00, context);
        emit(0x00, context);
        emit(0x00, context);
    }

    node->cc = false;
}


Scope* new_scope() {
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;
    scope->symbols = AS_TABLE(new_table((TValue[1]){ NIL }, 0));
    scope->const_idx = AS_TABLE(new_table((TValue[1]){ NIL }, 0));

    for (int i = 0; i < sizeof(scope->const_pool); i++) { scope->const_pool.const_pool[i] = NIL; }
    scope->const_pool.len = 0;

    scope->free_regs.len = 0;
    scope->reg_ptr = 0;
    scope->breaks.ptr = NULL;
    scope->breaks.next = NULL;
    scope->continues.ptr = NULL;
    scope->continues.next = NULL;

    scope->bytecode.bytes = malloc(64);
    scope->bytecode.len = 0;
    scope->bytecode.cap = 64;
    scope->bytecode.bytes[0] = T_EOF;

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
    if (idx != context->reg_ptr - 1) return (context->free_regs.free_regs[context->free_regs.len++] = idx);
    context->reg_ptr--;

    return idx;
}

static inline int cache_constant(TValue const_v, Scope* context) {
    TValue idx = get_value_from_hashtable(TBL_TO_TVALUE(context->const_idx), const_v);
    if (AS_TVALUE(idx) != AS_TVALUE(NIL)) return AS_NUM(idx);

    if (context->const_pool.len == 0xff) STACK_OVERFLOW("Fatal Error : Constant overflow, exceeded constants limit");
    context->const_pool.const_pool[context->const_pool.len++] = const_v;
    add_value_to_hashtable(TBL_TO_TVALUE(context->const_idx), const_v, NUM_AS_TVALUE(context->const_pool.len - 1));

    return context->const_pool.len - 1;
}

static inline int register_top(Scope* context) {
    if (context->reg_ptr == 0xff) STACK_OVERFLOW("Fatal Error : Register overflow, exceeded register limit");
    return context->reg_ptr++;
}

static inline int register_constant(uint8_t const_i, Scope* context, int16_t r_) {
    uint8_t r = r_ + 1 ? r_ : register_first(context);
    emit(LOAD_CR, context);
    emit(const_i, context);
    emit(r, context);
    emit(0x00, context);
}

static inline int register_register(uint8_t r, Scope* context, int16_t r_) {
    uint8_t r2 = r_ + 1 ? r_ : register_first(context);
    emit(LOAD_RR, context);
    emit(r, context);
    emit(r2, context);
    emit(0x00, context);
}