#ifndef AST_CLEANER_H
#define AST_CLEANER_H

#include "Parser/nodes.h"

typedef int (*CleanerFunction)(Node* node);

void clean_node(Node* node);
void clean_nothing(Node* node);
void clean_statements(Node* node);
void clean_var_declare(Node* node);
void clean_assignement(Node* node);
void clean_bin_op(Node* node);
void clean_uni_op(Node* node);
void clean_call(Node* node);
void clean_table(Node* node);
void clean_if_statement(Node* node);
void clean_nfor_loop(Node* node);
void clean_gfor_loop(Node* node);
void clean_while_loop(Node* node);
void clean_import(Node* node);
void clean_from_import(Node* node);
void clean_func_def(Node* node);
void clean_class_def(Node* node);
void clean_fieldaccess(Node* node);
void clean_indexing(Node* node);


#endif