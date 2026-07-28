#include "Globals/table.h"
#include "Compiler/class_mapping.h"
#include "TypeResolving/type_specifiers.h"

ClassMapper class_mappers[] = {
    [NT_STATEMENTS]     = classmap_statements,
    [NT_RETURN]         = classmap_return,
    [NT_BREAK]          = classmap_break,
    [NT_CONTINUE]       = classmap_continue,
    [NT_VAR_DECLARE]    = classmap_var_declare,
    [NT_ASSIGNEMENT]    = classmap_assignement,
    [NT_BIN_OP]         = classmap_bin_op,
    [NT_UNI_OP]         = classmap_uni_op,
    [NT_CALL]           = classmap_call,
    [NT_VAR_ACESS]      = classmap_var_access,
    [NT_NUMBER]         = classmap_number,
    [NT_STRING]         = classmap_string,
    [NT_TABLE]          = classmap_table,
    [NT_ALLOCATION]     = classmap_allocation,
    [NT_IF]             = classmap_if_statement,
    [NT_NFOR]           = classmap_nfor_loop,
    [NT_GFOR]           = classmap_gfor_loop,
    [NT_WHILE]          = classmap_while_loop,
    [NT_REPEAT]         = classmap_repeat_loop,
    [NT_REQUIRE]        = classmap_require,
    [NT_CREQUIRE]       = classmap_require,
    [NT_IMPORT]         = classmap_import,
    [NT_CIMPORT]        = classmap_import,
    [NT_FROM_IMPORT]    = classmap_from_import,
    [NT_FUNCDEF]        = classmap_func_def,
    [NT_METHODDEF]      = classmap_method_def,
    [NT_CLASSDEF]       = classmap_class_def,
    [NT_SELFCALL]       = classmap_selfcall,
    [NT_SELFDECLARE]    = classmap_selfdeclare,
    [NT_FIELDACCESS]    = classmap_fieldaccess,
    [NT_CONSTRUCTOR]    = classmap_method_def,
    [NT_INDEXING]       = classmap_indexing
};


void classmap(Node* node) {
    return class_mappers[node->nt](node);
}

void classmap_statements(Node* node) {
    for (int i = 0; i < node->node.StatementsNode.len; i++) classmap(node->node.StatementsNode.nodes[i]);
}

void classmap_return(Node* node) {
    for (int i = 0; i < node->node.ReturnNode.len; i++) classmap(node->node.ReturnNode.nodes[i]);
}

void classmap_break(Node* node) {}
void classmap_continue(Node* node) {}

void classmap_var_declare(Node* node) {
    if (node->node.VarDeclareNode.value) classmap(node->node.VarDeclareNode.value);
}

void classmap_assignement(Node* node) {
    classmap(node->node.AssignementNode.lvalue);
    classmap(node->node.AssignementNode.value);
}

void classmap_bin_op(Node* node) {
    Node* left = node->node.BinOpNode.left;
    Node* right = node->node.BinOpNode.right;
    classmap(left);
    classmap(right);

    switch (node->node.BinOpNode.op.tt) {
        case TT_EEQ:
        case TT_CONCAT:
            if ((EQUAL(left->type, varType) || EQUAL(left->type, StringType)) && ( EQUAL(right->type, varType) || EQUAL(right->type, StringType))) return;

        default:
            if ((EQUAL(left->type, varType) || EQUAL(left->type, NumberType)) && ( EQUAL(right->type, varType) || EQUAL(right->type, NumberType))) return;
    }

    TValue op = get_value_from_hashtable(operation_methods, NUM_AS_TVALUE(node->node.BinOpNode.op.tt));
    TValue rop = right_operation_method(node->node.BinOpNode.op.tt);
    TValue lmap = get_value_from_hashtable(globals_map, dunder(left->type));
    TValue rmap = get_value_from_hashtable(globals_map, dunder(right->type));

    TValue lnm = IS_NIL(lmap) ? NIL : get_namespace_map(lmap);
    TValue rnm = IS_NIL(rmap) ? NIL : get_namespace_map(rmap);
    TValue ldesc = IS_NIL(lnm) ? NIL : get_value_from_hashtable(lnm, op);
    TValue rdesc = IS_NIL(rnm) ? NIL : get_value_from_hashtable(rnm, rop);

    TValue finalOp = IS_NIL(ldesc) ? rop : op;
    Node* finalArg = IS_NIL(ldesc) ? left : right;
    Node* final_target = IS_NIL(ldesc) ? right : left;

    node->nt = NT_SELFCALL;
    node->node.SelfCallnode.name = op;
    node->node.SelfCallnode.target = final_target;
    node->node.SelfCallnode.args = make_node_array();
    append_to_node_array(&node->node.SelfCallnode.args, finalArg);
}

void classmap_uni_op(Node* node) {
    Node* target = node->node.UniOpNode.target;
    classmap(target);

    if (EQUAL(target->type, varType) || EQUAL(target->type, NumberType)) return;

    TValue op = get_value_from_hashtable(operation_methods, NUM_AS_TVALUE(node->node.UniOpNode.op.tt));
    TValue map = get_value_from_hashtable(globals_map, dunder(target->type));
    TValue nm = IS_NIL(map) ? NIL : get_namespace_map(map);
    TValue desc = IS_NIL(nm) ? NIL : get_value_from_hashtable(nm, op);

    if (IS_NIL(desc)) return;

    node->nt = NT_SELFCALL;
    node->node.SelfCallnode.name = op;
    node->node.SelfCallnode.target = target;
    node->node.SelfCallnode.args = make_node_array();
}

void classmap_call(Node* node) {
    classmap(node->node.CallNode.target);
    for (int i = 0; i < node->node.CallNode.args.len; i++) classmap(node->node.CallNode.args.nodes[i]);
}

void classmap_var_access(Node* node) {}

void classmap_number(Node* node) {}

void classmap_string(Node* node) {}

void classmap_table(Node* node) {
    for (int i = 0; i < node->node.TableNode.values.len; i++) classmap(node->node.TableNode.values.nodes[i]);
}

void classmap_allocation(Node* node) {
    classmap(node->node.AllocNode.target);
    for (int i = 0; i < node->node.AllocNode.args.len; i++) classmap(node->node.AllocNode.args.nodes[i]);
}

void classmap_if_statement(Node* node) {
    for (int i = 0; i < node->node.IfNode.cases.len; i++) classmap(node->node.IfNode.cases.nodes[i]);
    for (int i = 0; i < node->node.IfNode.statements.len; i++) classmap(node->node.IfNode.statements.nodes[i]);
    if (node->node.IfNode.statements.len > node->node.IfNode.cases.len) classmap(node->node.IfNode.statements.nodes[node->node.IfNode.statements.len - 1]);
}

void classmap_nfor_loop(Node* node) {
    classmap(node->node.nForNode.values[0]);
    classmap(node->node.nForNode.values[1]);
    classmap(node->node.nForNode.values[2]);
    classmap(node->node.nForNode.statements);
}

void classmap_gfor_loop(Node* node) {
    classmap(node->node.gForNode.target);
    classmap(node->node.gForNode.statements);
}

void classmap_while_loop(Node* node) {
    classmap(node->node.whileNode.condition);
    classmap(node->node.whileNode.statements);
}

void classmap_repeat_loop(Node* node) {
    classmap(node->node.repeatNode.condition);
    classmap(node->node.repeatNode.statements);
}

void classmap_require(Node* node) {
    classmap(node->node.importNode.ast);
}

void classmap_import(Node* node) {
    classmap(node->node.importNode.ast);
}

void classmap_from_import(Node* node) {
    classmap(node->node.fromImportNode.import_node);
}

void classmap_func_def(Node* node) {
    classmap(node->node.funcDefNode.statements);
}

void classmap_method_def(Node* node) {
    TValue class_namespace = get_value_from_hashtable(globals_map, node->node.methodDefNode.class);
    NamespaceEntry* class_entry = as_namespace_entry(class_namespace);
    TValue class_map = get_namespace_map(class_namespace);

    add_value_to_hashtable(class_map, node->node.methodDefNode.name, class_entry->value);
    class_entry->value.num++;

    classmap(node->node.methodDefNode.statements);
}

void classmap_class_def(Node* node) {
    TValue class_namespace = new_namespace_entry(NUM_AS_TVALUE(0), NULL);
    TValue class_obj_namespace = new_namespace_entry(NUM_AS_TVALUE(0), NULL);

    add_value_to_hashtable(globals_map, dunder(node->node.classDefNode.name), class_namespace);
    add_value_to_hashtable(globals_map, node->node.classDefNode.name, class_obj_namespace);

    classmap(node->node.classDefNode.statements);
}

void classmap_selfcall(Node* node) {
    classmap(node->node.SelfCallnode.target);
    for (int i = 0; i < node->node.SelfCallnode.args.len; i++) classmap(node->node.SelfCallnode.args.nodes[i]);
}

void classmap_selfdeclare(Node* node) {
    TValue class_namespace = get_value_from_hashtable(globals_map, node->node.SelfDeclarenode.class);
    NamespaceEntry* class_entry = as_namespace_entry(class_namespace);
    TValue class_map = get_namespace_map(class_namespace);

    add_value_to_hashtable(class_map, node->node.SelfDeclarenode.var_name, class_entry->value);
    class_entry->value.num++;

    if (node->node.SelfDeclarenode.value) classmap(node->node.SelfDeclarenode.value);
}

void classmap_fieldaccess(Node* node) {
    classmap(node->node.FieldAccessNode.target);
}

void classmap_indexing(Node* node) {
    Node* target = node->node.IndexNode.target;
    Node* index = node->node.IndexNode.index;

    classmap(target);
    classmap(node->node.IndexNode.index);

    if (EQUAL(target->type, varType) || EQUAL(target->type, TableType)) return;

    TValue __index = make_fstring("__index");
    TValue map = get_value_from_hashtable(globals_map, dunder(target->type));
    TValue nm = IS_NIL(map) ? NIL : get_namespace_map(map);
    TValue desc = IS_NIL(nm) ? NIL : get_value_from_hashtable(nm, __index);

    if (IS_NIL(desc)) return;

    node->nt = NT_SELFCALL;
    node->node.SelfCallnode.name = __index;
    node->node.SelfCallnode.target = target;
    node->node.SelfCallnode.args = make_node_array();

    append_to_node_array(&node->node.SelfCallnode.args, index);
}