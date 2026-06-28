#include "TypeResolving/type_resolving.h"
#include "Values/namespaces.h"
#include "Globals/string.h"
#include "Globals/table.h"
#include "Globals/tError.h"


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

    return varType;
}

TValue resolve_type_return(Node* node, TypesContext* context) {
    TValue return_type = context->return_types.next->value;
    if (node->node.ReturnNode.len == 0) return (node->type = varType);
    if (node->node.ReturnNode.len == 1) {
        TValue return_value = resolve_type(node->node.ReturnNode.nodes[0], context);

        if (EQUAL(return_type, return_value)) { return (node->type = return_type); }
        else {
            TValue error_message = make_fstring("Tried returning value of type <%s> in function of type <%s>");
            error_message = format_string((TValue[3]){ error_message, return_value, return_type }, 3);

            call_error(new_type_error, AS_STRING(error_message)->str);
        }
    }

    if (!EQUAL(return_type, tableType)) {
        TValue error_message = make_fstring("Tried returning multiple argument to function of type <%s>");
        error_message = format_string((TValue[2]){ error_message, return_type }, 2);
    }

    for (int i = 0; i < node->node.ReturnNode.len; i++) resolve_type(node->node.ReturnNode.nodes[i], context);
    return (node->type = tableType);
}

TValue resolve_type_break(Node* node, TypesContext* context) {
    return varType;
}

TValue resolve_type_continue(Node* node, TypesContext* context) {
    return varType;
}

TValue resolve_type_var_declare(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.VarDeclareNode.value, context);
    TValue var_type = node->node.VarDeclareNode.type_specifier;

    if (!EQUAL(var_type, varType) && !EQUAL(return_type, varType) && !EQUAL(var_type, return_type)) {
        TValue error_message = make_fstring("Cannot assign value of type <%s> to variable of type <%s>");
        error_message = format_string((TValue[3]){ error_message, return_type, var_type }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.VarDeclareNode.var_name, new_namespace_entry(var_type, NULL));
    return var_type;
}

TValue resolve_type_assignement(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.AssignementNode.value, context);
    TValue lvalue_type = resolve_type(node->node.AssignementNode.lvalue, context);
    if (EQUAL(lvalue_type, varType) || EQUAL(return_type, varType)) return varType;
    if (!EQUAL(lvalue_type, return_type)) {
        TValue error_message = make_fstring("Cannot assign value of type <%s> to lvalue of type <%s>");
        error_message = format_string((TValue[3]){ error_message, return_type, lvalue_type }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    return lvalue_type;
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
    MethodDescriptor* ldescPtr = IS_NIL(ldesc) ? NULL : (MethodDescriptor*)PAYLOAD(ldesc);
    MethodDescriptor* rdescPtr = IS_NIL(rdesc) ? NULL : (MethodDescriptor*)PAYLOAD(rdesc);

    if (ldescPtr && in_table(ldescPtr->arg_types, rtype)) return ldescPtr->return_type;
    if (rdescPtr && in_table(rdescPtr->arg_types, ltype)) return rdescPtr->return_type;

    TValue error_message = make_fstring("Cannot proceed to operation : %s with types : <%s> and <%s>");
    error_message = format_string((TValue[4]){ error_message, op, ltype, rtype }, 4);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}

TValue resolve_type_uni_op(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.UniOpNode.target, context);
    if (IS_VAR(return_type)) return varType;

    TValue op = get_value_from_hashtable(unary_operation_methods, NUM_AS_TVALUE(node->node.UniOpNode.op.tt));
    TValue map = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), return_type);
    TValue desc = get_value_from_hashtable(map, op);
    MethodDescriptor* descPtr = IS_NIL(desc) ? NULL : (MethodDescriptor*)PAYLOAD(desc);

    if (descPtr && in_table(descPtr->arg_types, return_type)) return descPtr->return_type;

    TValue error_message = make_fstring("Cannot proceed to unary operation : %s with type : <%s>");
    error_message = format_string((TValue[3]){ error_message, op, return_type }, 3);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}

TValue resolve_type_call(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.CallNode.target, context);
    if (EQUAL(target_type, varType)) return varType;
    if (!EQUAL(target_type, funcType)) {
        TValue error_message = make_fstring("Cannot call value of type <%s>");
        error_message = format_string((TValue[3]){ error_message, target_type }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }



}

TValue resolve_type_var_access(Node* node, TypesContext* context) {
    TValue entry = get_value_from_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.VarAcess.name);
    if (EQUAL(entry, NIL)) {
        TValue error_message = make_fstring("Unknwon identifier : <%s>");
        error_message = format_string((TValue[3]){ error_message, node->node.VarAcess.name }, 2);

        call_error(new_unknown_identifier_error, AS_STRING(error_message)->str);
    }

    NamespaceEntry* nm = as_namespace_entry(entry);
    return (node->type = nm->value);
}

TValue resolve_type_number(Node* node, TypesContext* context) {
    return (node->type = make_fstring("Number"));
}

TValue resolve_type_string(Node* node, TypesContext* context) {
    return (node->type = make_fstring("String"));
}

TValue resolve_type_allocation(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.AllocNode.target, context);
    for (int i = 0; i < node->node.AllocNode.args.len; i++) {
        resolve_type(node->node.AllocNode.args.nodes[i], context);
    }

    if (EQUAL(target_type, varType) || EQUAL(target_type, tableType)) return (node->type = tableType);
    call_error(new_type_error, "Allocation only work on type <Table>");
}

TValue resolve_type_if_statement(Node* node, TypesContext* context) {
    for (int i = 0; i < node->node.IfNode.cases.len; i++) {
        resolve_type(node->node.IfNode.cases.nodes[i], context);
    }

    for (int i = 0; i < node->node.IfNode.statements.len; i++) {
        resolve_type(node->node.IfNode.statements.nodes[i], context);
    }

    return (node->type = varType);
}

TValue resolve_type_nfor_loop(Node* node, TypesContext* context) {
    TValue v1 = resolve_type(node->node.nForNode.values[0], context),
    v2 = resolve_type(node->node.nForNode.values[1], context),
    v3 = resolve_type(node->node.nForNode.values[2], context);

    int v1cond = EQUAL(v1, varType) || EQUAL(v1, numType);
    int v2cond = EQUAL(v2, varType) || EQUAL(v2, numType);
    int v3cond = EQUAL(v3, varType) || EQUAL(v3, numType);

    if (!v1cond || !v2cond || !v3cond) {
        TValue error_message = make_fstring("numeric for loops can only use type 'num'");
        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    resolve_type(node->node.nForNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_gfor_loop(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.gForNode.target, context);

    if (!EQUAL(target_type, varType)) {
        TValue target_map = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), target_type);
        TValue iter_desc = get_value_from_hashtable(target_map, make_fstring("__iter"));

        if (IS_NIL(iter_desc)) {
            TValue error_message = make_fstring("Type <%s> do not have an __iter method");
            error_message = format_string((TValue[4]){ error_message, target_type }, 2);
            
            call_error(new_type_error, AS_STRING(error_message)->str);
        }
    }

    resolve_type(node->node.gForNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_while_loop(Node* node, TypesContext* context) {
    resolve_type(node->node.whileNode.condition, context);
    resolve_type(node->node.whileNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_repeat_loop(Node* node, TypesContext* context) {
    resolve_type(node->node.repeatNode.condition, context);
    resolve_type(node->node.repeatNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_import(Node* node, TypesContext* context) {

}

TValue resolve_type_func_def(Node* node, TypesContext* context) {
    TTable* id_namespace = context->id_namespace;
    context->id_namespace = AS_TABLE(new_table(NULL, 0));

    struct TValueLinkedList* return_type = malloc(sizeof(struct TValueLinkedList));
    return_type->value = node->node.funcDefNode.type_specifier;
    return_type->next = context->return_types.next;
    context->return_types.next = return_type;

    resolve_type(node->node.funcDefNode.statements, context);
    context->id_namespace = id_namespace;
    context->return_types.next = context->return_types.next->next;

    free(return_type);
    return (node->type = funcType);
}


TypesContext* new_resolve_context() {
    TypesContext* context = malloc(sizeof(TypesContext));
    context->return_types.next = malloc(sizeof(struct TValueLinkedList));
    context->type_namespace = AS_TABLE(new_table(NULL, 0));
    context->id_namespace = AS_TABLE(new_table(NULL, 0));
    context->return_types.next->value = varType;

    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), numType, num_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), strType, str_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), tableType, table_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), NumberType, num_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), StringType, str_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), TableType, table_map);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), funcType, func_map);

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
