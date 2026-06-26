#ifndef CONSTANT_FOLDING
#define CONSTANT_FOLDING

#include "Parser/nodes.h"

typedef TValue (*FoldingFunction)(Node* node);

TValue fold(Node* node);
TValue fold_statements(Node* node);
TValue fold_return(Node* node);
TValue fold_break(Node* node);
TValue fold_continue(Node* node);
TValue fold_var_declare(Node* node);
TValue fold_assignement(Node* node);
TValue fold_bin_op(Node* node);
TValue fold_uni_op(Node* node);
TValue fold_call(Node* node);
TValue fold_var_access(Node* node);
TValue fold_number(Node* node);
TValue fold_string(Node* node);
TValue fold_allocation(Node* node);
TValue fold_if_statement(Node* node);
TValue fold_nfor_loop(Node* node);
TValue fold_gfor_loop(Node* node);
TValue fold_while_loop(Node* node);
TValue fold_repeat_loop(Node* node);
TValue fold_import(Node* node);
TValue fold_func_def(Node* node);

#endif