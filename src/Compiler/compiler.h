#ifndef COMPILER_H
#define COMPILER_H

#include "Values/bytev.h"
#include "Values/helper_structs.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Parser/nodes.h"
#include "utils/utils.h"

typedef enum {
    OP_ADD_RC,
    OP_ADD_RR,
    OP_SUB_RC,
    OP_SUB_CR,
    OP_SUB_RR,
    OP_MUL_RC,
    OP_MUL_RR,
    OP_DIV_RC,
    OP_DIV_CR,
    OP_DIV_RR,
    OP_IDIV_RC,
    OP_IDIV_CR,
    OP_IDIV_RR,
    OP_MOD_RC,
    OP_MOD_CR,
    OP_MOD_RR,
    OP_POW_RC,
    OP_POW_CR,
    OP_POW_RR,
    OP_CONCAT_RC,
    OP_CONCAT_CR,
    OP_CONCAT_RR,
    OP_EEQ_RC,
    OP_EEQ_RR,
    OP_NEQ_RC,
    OP_NEQ_RR,
    OP_LT_RC,
    OP_LT_RR,
    OP_GT_RC,
    OP_GT_RR,
    OP_LTEQ_RC,
    OP_LTEQ_RR,
    OP_GTEQ_RC,
    OP_GTEQ_RR,
    OP_RAND_RC,
    OP_RAND_RR,
    OP_ROR_RC,
    OP_ROR_RR,
    OP_AND_RC,
    OP_AND_RR,
    OP_OR_RC,
    OP_OR_RR,
    OP_BAND_RC,
    OP_BAND_RR,
    OP_BOR_RC,
    OP_BOR_RR,
    OP_BXOR_RC,
    OP_BXOR_RR,
    OP_BNOT,
    OP_JUMP,
    OP_JFALSE,
    OP_NFOR_INIT,
    OP_NFOR_LOOP,
    OP_GFOR_INIT,
    OP_GFOR_LOOP,
    OP_LOAD_CR,
    OP_LOAD_RR,
    OP_LOAD_UP,
    OP_CALL,
    OP_RETURN,
    OP_NEWTABLE,
    OP_ADDTABLE,
    OP_GETTABLE,
    OP_SETTABLE,
    OP_HALT
} BytecodeInstruction;


struct ContextScope;
typedef struct ContextScope {
    struct ContextScope* parent;
    struct { TValue saved_consts[256]; uint8_t len; } saved_const;
    struct { TValue const_pool[256]; uint8_t len; } const_pool;
    struct { uint8_t free_regs[256]; uint8_t len; } free_regs;
    struct { uint8_t* bytes; size_t len; size_t cap; } bytecode;
    struct uint8LinkedList breaks;
    struct uint8LinkedList continues;
    TTable* symbol_stack;
    TTable* symbols;
    uint8_t reg_ptr;
} Scope;

typedef int (*CompileFunction)(Node* node, Scope* context);
Scope* new_scope();

int compile(Node* node, Scope* context);
int compile_statements(Node* node, Scope* context);
int compile_return(Node* node, Scope* context);
int compile_break(Node* node, Scope* context);
int compile_continue(Node* node, Scope* context);
int compile_var_declare(Node* node, Scope* context);
int compile_assignement(Node* node, Scope* context);
int compile_bin_op(Node* node, Scope* context);
int compile_uni_op(Node* node, Scope* context);
int compile_call(Node* node, Scope* context);
int compile_var_access(Node* node, Scope* context);
int compile_number(Node* node, Scope* context);
int compile_string(Node* node, Scope* context);
int compile_allocation(Node* node, Scope* context);
int compile_if_statement(Node* node, Scope* context);
int compile_nfor_loop(Node* node, Scope* context);
int compile_gfor_loop(Node* node, Scope* context);
int compile_while_loop(Node* node, Scope* context);
int compile_repeat_loop(Node* node, Scope* context);
int compile_import(Node* node, Scope* context);
int compile_func_def(Node* node, Scope* context);

int emit(uint8_t instr, Scope* context);
static int register_first(Scope* context);
static int free_register(uint8_t idx, Scope* context);
static int register_constant(uint8_t const_i, Scope* context, int16_t r_);
static int register_register(uint8_t r, Scope* context, int16_t r_);
static int register_try_free(uint8_t r, Scope* context);
static int register_try_newtop(uint8_t r, Scope* context);
static int free_multiple(uint8_t r0, uint8_t len, Scope* context);
static int get_constant_idx(TValue const_v, Scope* context);
static int cache_constant(TValue const_v, Scope* context);
static int is_saved(TValue const_v, Scope* context);
static inline int register_top(Scope* context);
static inline int register_specific(uint8_t r, Scope* context);
static inline int register_try(uint8_t r, Scope* context);
static inline int save_cached_constant(int idx, Scope* context);
static inline int correct_constant_idx(int idx, Scope* context);
static inline void sure_delete_constant(int idx, Scope* context);
static void delete_constant(int idx, Scope* context);
static void unsave_cached_constant(int idx, Scope* context);
static inline TValue save_constant(TValue const_v, Scope* context);
static inline TValue get_constant(int idx, Scope* context);
static TValue operation_fold(int idx1, TokenType op, int idx2, Scope* context);
static inline int assignement_analysis(Node* node);
static inline int assign_to_reg(Node* node, uint8_t r, Scope* context);
static int full_assignement_analysis(Node* node, Node* target, int* depth, int* target_count);

static void open_scope(Scope* context);
static void close_scope(Scope* context);
static inline int compile_in_scope(Node* node, Scope* context);
static inline void update_symbols(Scope* context);
static TValue append_symbol(Scope* context, TValue name, int r);
static TValue get_symbol_register(Scope* context, TValue name);

#endif