#ifndef COMPILER_H
#define COMPILER_H

#include "Values/bytev.h"
#include "Values/helper_structs.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Parser/nodes.h"
#include "utils/utils.h"
#include "Compiler/instructions.h"

struct ContextScope;
typedef struct ContextScope {
    struct ContextScope* parent;
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
int compile_table(Node* node, Scope* context);
int compile_allocation(Node* node, Scope* context);
int compile_if_statement(Node* node, Scope* context);
int compile_nfor_loop(Node* node, Scope* context);
int compile_gfor_loop(Node* node, Scope* context);
int compile_while_loop(Node* node, Scope* context);
int compile_repeat_loop(Node* node, Scope* context);
int compile_import(Node* node, Scope* context);
int compile_func_def(Node* node, Scope* context);
int compile_method_def(Node* node, Scope* context);
int compile_class_def(Node* node, Scope* context);
int compile_class_def(Node* node, Scope* context);
int compile_selfcall(Node* node, Scope* context);
int compile_selfdeclare(Node* node, Scope* context);
int compile_fieldaccess(Node* node, Scope* context);
int compile_constructor(Node* node, Scope* context);
int compile_indexing(Node* node, Scope* context);

int emit(uint8_t instr, Scope* context);
static int register_first(Scope* context);
static int free_register(uint8_t idx, Scope* context);
static int register_constant(uint8_t const_i, Scope* context, int16_t r_);
static int register_register(uint8_t r, Scope* context, int16_t r_);
static int register_global(uint16_t glob_i, Scope* context, int16_t r_);
static int register_try_free(uint8_t r, Scope* context);
static int register_try_newtop(uint8_t r, Scope* context);
static int register_specific_top(uint8_t r, Scope* context);
static int free_multiple(uint8_t r0, uint8_t len, Scope* context);
static int get_constant_idx(TValue const_v, Scope* context);
static int cache_constant(TValue const_v, Scope* context);
static inline int register_top(Scope* context);
static inline int register_specific(uint8_t r, Scope* context);
static inline int register_try(uint8_t r, Scope* context);
static inline int correct_constant_idx(int idx, Scope* context);
static void delete_constant(int idx, Scope* context);
static inline TValue get_constant(int idx, Scope* context);
static inline int assignement_analysis(Node* node);
static inline int assign_to_reg(Node* node, uint8_t r, Scope* context);
static int full_assignement_analysis(Node* node, Node* target, int* depth, int* target_count);

static void open_scope(Scope* context);
static void close_scope(Scope* context);
static inline int compile_in_scope(Node* node, Scope* context);
static inline void update_symbols(Scope* context);
static TValue append_symbol(Scope* context, TValue name, int r);
static TValue get_symbol_register(Scope* context, TValue name);

int compile_boolean_op(int left, int right, int leftCC, int rightCC, TokenType op, Scope* context);

TValue close_context(Scope* context);

#endif