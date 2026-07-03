#ifndef CLASS_MAPPING_H
#define CLASS_MAPPING_H

#include "Values/bytev.h"
#include "Parser/nodes.h"

typedef void (*ClassMapper)(Node* node);

void classmap(Node* node);
void classmap_statements(Node* node);
void classmap_return(Node* node);
void classmap_break(Node* node);
void classmap_continue(Node* node);
void classmap_var_declare(Node* node);
void classmap_assignement(Node* node);
void classmap_bin_op(Node* node);
void classmap_uni_op(Node* node);
void classmap_call(Node* node);
void classmap_var_access(Node* node);
void classmap_number(Node* node);
void classmap_string(Node* node);
void classmap_table(Node* node);
void classmap_allocation(Node* node);
void classmap_if_statement(Node* node);
void classmap_nfor_loop(Node* node);
void classmap_gfor_loop(Node* node);
void classmap_while_loop(Node* node);
void classmap_repeat_loop(Node* node);
void classmap_import(Node* node);
void classmap_func_def(Node* node);
void classmap_method_def(Node* node);
void classmap_class_def(Node* node);
void classmap_selfcall(Node* node);
void classmap_selfdeclare(Node* node);
void classmap_fieldaccess(Node* node);
void classmap_constructor(Node* node);
void classmap_indexing(Node* node);

#endif