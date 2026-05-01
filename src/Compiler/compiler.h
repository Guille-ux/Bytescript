#ifndef COMPILER_H
#define COMPILER_H


#include "Values/bytev.h"
#include "Globals/table.h"


typedef enum {
    ADD_RC,
    ADD_RR,
    SUB_RC,
    SUB_RR,
    MUL_RC,
    MUL_RR,
    DIV_RC,
    DIV_CR,
    DIV_RR,
    IDIV_RC,
    IDIV_CR,
    IDIV_RR,
    MOD_RC,
    MOD_CR,
    MOD_RR,
    POW_RC,
    POW_RR,
    CONCAT_RC,
    CONCAT_CR,
    CONCAT_RR,
    EEQ_RC,
    EEQ_RR,
    NEQ_RC,
    NEQ_RR,
    LT_RC,
    LT_RR,
    GT_RC,
    GT_RR,
    LTEQ_RC,
    LTEQ_RR,
    GTEQ_RC,
    GTEQ_RR,
    RAND_RC,
    RAND_RR,
    ROR_RC,
    ROR_RR,
    AND_RC,
    AND_RR,
    OR_RC,
    OR_RR,
    BAND_RC,
    BAND_RR,
    BOR_RC,
    BOR_RR,
    BXOR_RC,
    BXOR_RR,
    BNOT,
    JUMP,
    JFALSE,
    NFOR_INIT,
    NFOR_LOOP,
    GFOR_INIT,
    GFOR_LOOP,
    LOAD_CR,
    LOAD_RR,
    LOAD_UP,
    CALL,
    RETURN,
    NEWTABLE,
    ADDTABLE,
    GETTABLE,
    SETTABLE,
    T_EOF
} BytecodeInstruction;


struct LinkedList {
    uint8_t* ptr;
    struct LinkedList* next;
};

typedef struct {
    Scope* parent;
    TTable* symbols;
    TTable* const_idx;
    struct { TValue const_pool[256]; uint8_t len; } const_pool;
    struct { uint8_t free_regs[256]; uint8_t len; } free_regs;
    uint8_t reg_ptr;
    struct LinkedList breaks;
    struct LinkedList continues;
    struct { uint8_t* bytes; size_t len; size_t cap } bytecode;
} Scope;

typedef uint8_t (*CompileFunction)(Node* node, Scope* context);
Scope* new_scope();

inline int compile(Node* node, Scope* context);
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

static inline int emit(uint8_t instr, Scope* context);
static inline int register_first(Scope* context);
static inline int free_register(uint8_t idx, Scope* context);
static inline int cache_constant(TValue const_v, Scope* context);
static inline int register_top(Scope* context);
static inline int register_constant(uint8_t r, Scope* context, int16_t r_);
static inline int register_register(uint8_t r, Scope* context, int16_t r_);
static inline int register_specific(uint8_t r, Scope* context);

#endif