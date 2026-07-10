#ifndef TYPE_RESOLVING_H
#define TYPE_RESOLVING_H

#include "Parser/nodes.h"
#include "TypeResolving/type_specifiers.h"

extern TValue type_namespace;
typedef struct {
    TValue current_class;
    TValue return_type;
    TTable* func_namespace;
    TTable* func_stack;
    TTable* id_namespace;
    TTable* id_stack;
} TypesContext;

typedef TValue (*TypeResolver)(Node* node, TypesContext* context);

#define IS_VAR(tv)      ( EQUAL(tv, varType) )

TValue resolve_type(Node* node, TypesContext* context);
TValue resolve_type_statements(Node* node, TypesContext* context);
TValue resolve_type_return(Node* node, TypesContext* context);
TValue resolve_type_break(Node* node, TypesContext* context);
TValue resolve_type_continue(Node* node, TypesContext* context);
TValue resolve_type_var_declare(Node* node, TypesContext* context);
TValue resolve_type_assignement(Node* node, TypesContext* context);
TValue resolve_type_bin_op(Node* node, TypesContext* context);
TValue resolve_type_uni_op(Node* node, TypesContext* context);
TValue resolve_type_call(Node* node, TypesContext* context);
TValue resolve_type_var_access(Node* node, TypesContext* context);
TValue resolve_type_number(Node* node, TypesContext* context);
TValue resolve_type_string(Node* node, TypesContext* context);
TValue resolve_type_table(Node* node, TypesContext* context);
TValue resolve_type_allocation(Node* node, TypesContext* context);
TValue resolve_type_if_statement(Node* node, TypesContext* context);
TValue resolve_type_nfor_loop(Node* node, TypesContext* context);
TValue resolve_type_gfor_loop(Node* node, TypesContext* context);
TValue resolve_type_while_loop(Node* node, TypesContext* context);
TValue resolve_type_repeat_loop(Node* node, TypesContext* context);
TValue resolve_type_require(Node* node, TypesContext* context);
TValue resolve_type_import(Node* node, TypesContext* context);
TValue resolve_type_from_import(Node* node, TypesContext* context);
TValue resolve_type_func_def(Node* node, TypesContext* context);
TValue resolve_type_method_def(Node* node, TypesContext* context);
TValue resolve_type_class_def(Node* node, TypesContext* context);
TValue resolve_type_selfcall(Node* node, TypesContext* context);
TValue resolve_type_selfdeclare(Node* node, TypesContext* context);
TValue resolve_type_fieldaccess(Node* node, TypesContext* context);
TValue resolve_type_constructor(Node* node, TypesContext* context);
TValue resolve_type_indexing(Node* node, TypesContext* context);

TypesContext* new_resolve_context();
TValue check_call_types(Node* call_node, TTable* expecteds, TypesContext* context, TValue func_name);
static TValue get_symbol_type(TValue name, TypesContext* context);
static TValue get_func_descriptor(TValue name, TypesContext* context);
static inline TValue resolve_in_scope(Node* node, TypesContext* context);
static void open_type_scope(TypesContext* context);
static void close_type_scope(TypesContext* context);
static inline void update_types(TypesContext* context);

#endif