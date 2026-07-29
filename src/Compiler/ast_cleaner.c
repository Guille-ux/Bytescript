#include "Compiler/ast_cleaner.h"
#include "Values/tutils.h"

CleanerFunction cleaner_functions[] = {
    [NT_STATEMENTS]     = clean_statements,
    [NT_RETURN]         = clean_statements,
    [NT_BREAK]          = clean_nothing,
    [NT_CONTINUE]       = clean_nothing,
    [NT_VAR_DECLARE]    = clean_var_declare,
    [NT_ASSIGNEMENT]    = clean_assignement,
    [NT_BIN_OP]         = clean_bin_op,
    [NT_UNI_OP]         = clean_uni_op,
    [NT_CALL]           = clean_call,
    [NT_VAR_ACESS]      = clean_nothing,
    [NT_NUMBER]         = clean_nothing,
    [NT_STRING]         = clean_nothing,
    [NT_TABLE]          = clean_table,
    [NT_ALLOCATION]     = clean_call,
    [NT_IF]             = clean_if_statement,
    [NT_NFOR]           = clean_nfor_loop,
    [NT_GFOR]           = clean_gfor_loop,
    [NT_WHILE]          = clean_while_loop,
    [NT_REPEAT]         = clean_while_loop,
    [NT_REQUIRE]        = clean_import,
    [NT_CREQUIRE]       = clean_import,
    [NT_IMPORT]         = clean_import,
    [NT_CIMPORT]        = clean_import,
    [NT_FROM_IMPORT]    = clean_from_import,
    [NT_FUNCDEF]        = clean_func_def,
    [NT_METHODDEF]      = clean_func_def,
    [NT_CLASSDEF]       = clean_class_def,
    [NT_SELFCALL]       = clean_call,
    [NT_SELFDECLARE]    = clean_var_declare,
    [NT_FIELDACCESS]    = clean_fieldaccess,
    [NT_CONSTRUCTOR]    = clean_func_def,
    [NT_INDEXING]       = clean_indexing
};


void clean_node(Node* node) {
    cleaner_functions[node->nt](node);
    free(node);
}

void clean_nothing(Node* node) {}

void clean_statements(Node* node) {
    for (int i = 0; i < node->node.StatementsNode.len; i++) clean_node(node->node.StatementsNode.nodes[i]);
    free(node->node.StatementsNode.nodes);
}

void clean_var_declare(Node* node) { clean_node(node->node.VarDeclareNode.value); }

void clean_assignement(Node* node) {
    clean_node(node->node.AssignementNode.lvalue);
    clean_node(node->node.AssignementNode.value);
}

void clean_bin_op(Node* node) {
    clean_node(node->node.BinOpNode.left);
    clean_node(node->node.BinOpNode.right);
}

void clean_uni_op(Node* node) { clean_node(node->node.UniOpNode.target); }

void clean_call(Node* node) {
    for (int i = 0; i < node->node.CallNode.args.len; i++) clean_node(node->node.CallNode.args.nodes[i]); 
    free(node->node.CallNode.args.nodes);
    clean_node(node->node.CallNode.target);
}

void clean_table(Node* node) {
    for (int i = 0; i < node->node.TableNode.values.len; i++) clean_node(node->node.TableNode.values.nodes[i]);
    free(node->node.TableNode.values.nodes);
}

void clean_if_statement(Node* node) {
    for (int i = 0; i < node->node.IfNode.cases.len; i++) clea_node(node->node.IfNode.cases.nodes[i]);
    for (int i = 0; i < node->node.IfNode.statements.len; i++) clea_node(node->node.IfNode.statements.nodes[i]);
    free(node->node.IfNode.cases.nodes);
    free(node->node.IfNode.statements.nodes);
}

void clean_nfor_loop(Node* node) {
    clean_node(node->node.nForNode.statements);
    clean_node(node->node.nForNode.values[0]);
    clean_node(node->node.nForNode.values[1]);
    clean_node(node->node.nForNode.values[2]);
}

void clean_gfor_loop(Node* node) {
    clean_node(node->node.gForNode.statements);
    clean_node(node->node.gForNode.target);
}

void clean_while_loop(Node* node) {
    clean_node(node->node.whileNode.condition);
    clean_node(node->node.whileNode.statements);
}

void clean_import(Node* node) { clean_node(node->node.importNode.ast); }

void clean_from_import(Node* node) {
    free_linked_list(node->node.fromImportNode.elem_types);
    free_linked_list(node->node.fromImportNode.elements);
    free_linked_list(node->node.fromImportNode.nicknames);
    clean_node(node->node.fromImportNode.import_node);
}

void clean_func_def(Node* node) {
    free_linked_list(node->node.funcDefNode.args);
    free_linked_list(node->node.funcDefNode.arg_types);
    clean_node(node->node.funcDefNode.statements);
}

void clean_class_def(Node* node) {
    free_linked_list(node->node.classDefNode.inherited);
    clean_node(node->node.classDefNode.statements);
}

void clean_fieldaccess(Node* node) { clean_node(node->node.FieldAccessNode.target); }

void clean_indexing(Node* node) {
    clean_node(node->node.IndexNode.index);
    clean_node(node->node.IndexNode.target);
}
