#include "TypeResolving/type_resolving.h"
#include "Values/namespaces.h"
#include "Globals/string.h"
#include "Globals/table.h"
#include "Error/error.h"

TValue operation_methods;
TCDataType OperationDescriptor_Type = { .name = "OperationDescriptor" }, *OperationDescriptorType = &OperationDescriptor_Type;
TypeResolver type_resolvers[] = {
    [NT_STATEMENTS]     = resolve_type_statements,
    [NT_RETURN]         = resolve_type_return,
    [NT_BREAK]          = resolve_type_break,
    [NT_CONTINUE]       = resolve_type_continue,
    [NT_VAR_DECLARE]    = resolve_type_var_declare,
    [NT_ASSIGNEMENT]    = resolve_type_assignement,
    [NT_BIN_OP]         = resolve_type_bin_op,
    [NT_UNI_OP]         = resolve_type_uni_op,
    [NT_CALL]           = resolve_type_call,
    [NT_VAR_ACESS]      = resolve_type_var_access,
    [NT_NUMBER]         = resolve_type_number,
    [NT_STRING]         = resolve_type_string,
    [NT_ALLOCATION]     = resolve_type_allocation,
    [NT_IF]             = resolve_type_if_statement,
    [NT_NFOR]           = resolve_type_nfor_loop,
    [NT_GFOR]           = resolve_type_gfor_loop,
    [NT_WHILE]          = resolve_type_while_loop,
    [NT_REPEAT]         = resolve_type_repeat_loop,
    [NT_IMPORT]         = resolve_type_import,
    [NT_FUNCDEF]        = resolve_type_func_def
};


TValue resolve_type(Node* node, TypesContext* context) {
    return type_resolvers[node->nt](node, context);
}

TValue resolve_type_statements(Node* node, TypesContext* context) {
    for (int i = 0; i < node->node.StatementsNode.len; i++) {
        resolve_type(node->node.StatementsNode.nodes[i], context);
    }
}

TValue resolve_type_return(Node* node, TypesContext* context) {

}

TValue resolve_type_break(Node* node, TypesContext* context) {

}

TValue resolve_type_continue(Node* node, TypesContext* context) {

}

TValue resolve_type_var_declare(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.VarDeclareNode.value, context);
    if (EQUAL(node->node.VarDeclareNode.type_specifier, varType)) return varType;

    
}

TValue resolve_type_assignement(Node* node, TypesContext* context) {

}

TValue resolve_type_bin_op(Node* node, TypesContext* context) {
    TValue ltype = resolve_type(node->node.BinOpNode.left, context);
    TValue rtype = resolve_type(node->node.BinOpNode.right, context);
    if (IS_VAR(ltype) || IS_VAR(rtype)) return varType;

    TValue op = get_value_from_hashtable(operation_methods, NUM_AS_TVALUE(node->node.BinOpNode.op.tt));
    TValue rop = right_operation_method(node->node.BinOpNode.op.tt);
    TValue lmap = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), ltype);
    TValue rmap = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), rtype);
    TValue ldesc = get_value_from_hashtable(lmap, op);
    TValue rdesc = get_value_from_hashtable(rmap, rop);
    OperationDescriptor* ldescPtr = IS_NIL(ldesc) ? NULL : (OperationDescriptor*)PAYLOAD(ldesc);
    OperationDescriptor* rdescPtr = IS_NIL(rdesc) ? NULL : (OperationDescriptor*)PAYLOAD(rdesc);

    if (ldescPtr && in_table(ldescPtr->valid_table, rtype)) return ldescPtr->return_type;
    if (rdescPtr && in_table(rdescPtr->valid_table, ltype)) return rdescPtr->return_type;

    char details[snprintf(NULL, 0, "Cannot proceed to operation : %s with types : '%s' and '%s'", AS_STRING(op)->str, AS_STRING(ltype)->str, AS_STRING(rtype)->str) + 1];
    snprintf(details, sizeof(details), "Cannot proceed to operation : %s with types : '%s' and '%s'", AS_STRING(op)->str, AS_STRING(ltype)->str, AS_STRING(rtype)->str);
    TYPE_ERROR(details);
}

TValue resolve_type_uni_op(Node* node, TypesContext* context) {

}

TValue resolve_type_call(Node* node, TypesContext* context) {

}

TValue resolve_type_var_access(Node* node, TypesContext* context) {

}

TValue resolve_type_number(Node* node, TypesContext* context) {
    return (node->type = make_fstring("num"));
}

TValue resolve_type_string(Node* node, TypesContext* context) {
    return (node->type = make_fstring("str"));
}

TValue resolve_type_allocation(Node* node, TypesContext* context) {

}

TValue resolve_type_if_statement(Node* node, TypesContext* context) {

}

TValue resolve_type_nfor_loop(Node* node, TypesContext* context) {

}

TValue resolve_type_gfor_loop(Node* node, TypesContext* context) {

}

TValue resolve_type_while_loop(Node* node, TypesContext* context) {

}

TValue resolve_type_repeat_loop(Node* node, TypesContext* context) {

}

TValue resolve_type_import(Node* node, TypesContext* context) {

}

TValue resolve_type_func_def(Node* node, TypesContext* context) {

}


TypesContext* new_resolve_context() {
    TypesContext* context = malloc(sizeof(TypesContext));
    context->type_namespace = AS_TABLE(new_table(NULL, 0));

    TValue num_map = new_table(NULL, 0);
    TValue num_descriptor = make_operation_descriptor((TValue[1]){ numType }, numType, 1);
    add_value_to_hashtable(num_map, make_fstring("__add"), num_descriptor);

    TValue str_map = new_table(NULL, 0);
    TValue str_descriptor = make_operation_descriptor((TValue[1]){ strType }, strType, 1);
    add_value_to_hashtable(num_map, make_fstring("__concat"), str_descriptor);

    TValue table_map = new_table(NULL, 0);
    TValue func_map = new_table(NULL, 0);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), numType, num_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), strType, str_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), make_fstring("table"), table_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), make_fstring("func"), func_map);

    operation_methods = new_table(NULL, 0);
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_PLUS),   make_fstring("__add"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_MINUS),  make_fstring("__sub"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_MUL),    make_fstring("__mul"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_POW),    make_fstring("__pow"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_DIV),    make_fstring("__div"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_IDIV),   make_fstring("__idiv"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_MOD),    make_fstring("__mod"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_CONCAT), make_fstring("__concat"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_EEQ),    make_fstring("__eq"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_NEQ),    make_fstring("__neq"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_NOT),    make_fstring("__not"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_LT),     make_fstring("__lt"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_GT),     make_fstring("__gt"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_LTEQ),   make_fstring("__lteq"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_GTEQ),   make_fstring("__gteq"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_RAND),   make_fstring("__rand"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_ROR),    make_fstring("__ror"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_AND),    make_fstring("__and"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_OR),     make_fstring("__or"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_BAND),   make_fstring("__band"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_BOR),    make_fstring("__bor"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_LSHIFT), make_fstring("__lshift"));
    add_value_to_hashtable(operation_methods, NUM_AS_TVALUE(TT_RSHIFT), make_fstring("__rshift"));

    return context;
}

TValue right_operation_method(TokenType op) {
    TValue op_method = get_value_from_hashtable(operation_methods, NUM_AS_TVALUE(op));
    TStr* ptr = AS_STRING(op_method);

    char buffer[ptr->len + 2];
    buffer[0] = '_'; buffer[1] = '_'; buffer[2] = 'r';

    strcpy(buffer + 3, ptr->str + 2);
    return make_fstring(buffer);
}

TValue make_operation_descriptor(TValue valids[], TValue return_type, size_t valid_num) {
    OperationDescriptor* res_descriptor = malloc(sizeof(OperationDescriptor));
    res_descriptor->cdata_type = OperationDescriptorType;
    res_descriptor->valid_table = new_table(valids, valid_num);
    res_descriptor->return_type = return_type;

    return CDT_TO_TVALUE(res_descriptor);
}
