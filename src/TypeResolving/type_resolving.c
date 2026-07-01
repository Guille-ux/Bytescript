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
    [NT_FUNCDEF]        = resolve_type_func_def,
    [NT_METHODDEF]      = resolve_type_func_def,
    [NT_CLASSDEF]       = resolve_type_class_def,
    [NT_SELFCALL]       = resolve_type_selfcall
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
    TValue return_type = context->return_type;
    if (node->node.ReturnNode.len == 0) return (node->type = varType);
    if (node->node.ReturnNode.len == 1) {
        TValue return_value = resolve_type(node->node.ReturnNode.nodes[0], context);

        if (EQUAL(return_type, return_value) || EQUAL(return_value, varType) || EQUAL(return_type, varType)) { return (node->type = return_type); }
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

    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.VarDeclareNode.var_name, var_type);
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
    TValue lcdesc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), ltype);
    TValue rcdesc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), rtype);
    TValue lmap = TBL_TO_TVALUE(as_class_descriptor(lcdesc)->id_namespace);
    TValue rmap = TBL_TO_TVALUE(as_class_descriptor(rcdesc)->id_namespace);
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
    TValue cdesc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), return_type);
    TValue map = TBL_TO_TVALUE(as_class_descriptor(cdesc)->id_namespace);
    TValue desc = get_value_from_hashtable(map, op);
    MethodDescriptor* descPtr = IS_NIL(desc) ? NULL : (MethodDescriptor*)PAYLOAD(desc);

    if (descPtr && in_table(descPtr->arg_types, return_type)) return descPtr->return_type;

    TValue error_message = make_fstring("Cannot proceed to unary operation : %s with type : <%s>");
    error_message = format_string((TValue[3]){ error_message, op, return_type }, 3);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}

TValue resolve_type_call(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.CallNode.target, context);

    if (!EQUAL(target_type, funcType) && !EQUAL(target_type, varType)) {
        TValue error_message = make_fstring("Cannot call value of type <%s>");
        error_message = format_string((TValue[3]){ error_message, target_type }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    if (node->node.CallNode.target->nt == NT_VAR_ACESS) {
        TValue target_desc = get_value_from_hashtable(TBL_TO_TVALUE(context->func_namespace), node->node.CallNode.target->node.VarAcess.name);
        if (!IS_NIL(target_desc)) {
            MethodDescriptor* desc = as_method_descriptor(target_desc);
            TTable* args = AS_TABLE(desc->arg_types);

            check_call_types(node, args, context);
            return desc->return_type;
        }
    }

    return varType;
}

TValue resolve_type_var_access(Node* node, TypesContext* context) {
    TValue var_type = get_value_from_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.VarAcess.name);
    if (EQUAL(var_type, NIL)) {
        TValue error_message = make_fstring("Unknown identifier : '%s'");
        error_message = format_string((TValue[3]){ error_message, node->node.VarAcess.name }, 2);

        call_error(new_unknown_identifier_error, AS_STRING(error_message)->str);
    }

    return (node->type = var_type);
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
        TValue target_cdesc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), target_type);
        TValue target_map = TBL_TO_TVALUE(as_class_descriptor(target_cdesc)->id_namespace);
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
    TValue pre_return_type = context->return_type;
    TValue pre_class = context->current_class;
    TTable* id_namespace = context->id_namespace;
    TTable* func_namespace = context->func_namespace;

    context->id_namespace = AS_TABLE(new_table(NULL, 0));
    context->func_namespace = AS_TABLE(new_table(NULL, 0));
    context->return_type = node->node.funcDefNode.type_specifier;

    if (node->nt == NT_METHODDEF) {
        context->current_class = node->node.methodDefNode.class;
        add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), make_fstring("self"), node->node.methodDefNode.class);
    }

    int argc = 0;
    struct TValueLinkedList* arg = node->node.funcDefNode.args.next;
    struct TValueLinkedList* arg_type = node->node.funcDefNode.arg_types.next;
    for (; arg; (arg = arg->next, arg_type = arg_type->next)) { add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), arg->value, arg_type->value); argc++; }

    resolve_type(node->node.funcDefNode.statements, context);
    context->id_namespace = id_namespace;
    context->func_namespace = func_namespace;
    context->return_type = pre_return_type;
    context->current_class = pre_class;

    if (!IS_NIL(node->node.funcDefNode.name)) {
        TValue arg_list[argc];
        struct TValueLinkedList* arg = node->node.funcDefNode.arg_types.next;
        for (int i = 0; i < argc; (arg = arg->next ,i++)) { arg_list[i] = arg->value; }

        TValue desc = make_method_descriptor(arg_list, node->node.funcDefNode.type_specifier, argc);

        if (node->nt == NT_METHODDEF) {
            TValue class_desc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), node->node.methodDefNode.class);
            TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->id_namespace);
            TValue class_func = TBL_TO_TVALUE(as_class_descriptor(class_desc)->func_namespace);
            
            add_value_to_hashtable(class_map, node->node.funcDefNode.name, funcType);
            add_value_to_hashtable(class_func, node->node.funcDefNode.name, desc);
        } else {
            add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.funcDefNode.name, funcType);
            add_value_to_hashtable(TBL_TO_TVALUE(context->func_namespace), node->node.funcDefNode.name, desc);
        }
    }

    return (node->type = funcType);
}

TValue resolve_type_class_def(Node* node, TypesContext* context) {
    TStr* name = AS_STRING(node->node.classDefNode.name);
    TValue pre_class = context->current_class;

    context->current_class = node->node.classDefNode.name;
    if (name->str[0] == '_' && name->str[1] == '_') {
        TValue error_message = make_fstring("Invalid class name <%s>, class name cannot start with '__'");
        error_message = format_string((TValue[2]){ error_message, context->current_class }, 2);

        call_error(new_invalid_identifier_error, AS_STRING(error_message)->str);
    }

    TValue dunder_class = dunder(context->current_class);

    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), context->current_class, dunder_class);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), dunder_class, make_class_descriptor(NIL));
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), context->current_class, make_class_descriptor(NIL));

    context->current_class = dunder_class;
    for (int i = 0; i < node->node.classDefNode.statements->node.StatementsNode.len; i++) {
        Node* cur = node->node.classDefNode.statements->node.StatementsNode.nodes[i];
        if (cur->nt == NT_FUNCDEF) {
            cur->nt = NT_METHODDEF;
            cur->node.methodDefNode.class = node->node.classDefNode.name;
        }

        resolve_type(cur, context);
    }

    context->current_class = pre_class;
    return (node->type = dunder_class);
}

TValue resolve_type_selfcall(Node* node, TypesContext* context) {
    TValue target = resolve_type(node->node.SelfCallnode.target, context);
    TValue name = node->node.SelfCallnode.name;

    if (EQUAL(target, varType)) call_error(new_type_error, "Must specify type of value in order to self-call");
    TValue class_desc = get_value_from_hashtable(TBL_TO_TVALUE(context->type_namespace), target);
    TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->id_namespace);

    if (IS_NIL(class_map)) {
        TValue error_message = make_fstring("Type <%s> does not have a proper method map");
        error_message = format_string((TValue[2]){ error_message, target }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue method = get_value_from_hashtable(TBL_TO_TVALUE(as_class_descriptor(class_desc)->func_namespace), name);

    if (IS_NIL(method)) {
        TValue error_message = make_fstring("Type <%s> does not have any method of name <%s>");
        error_message = format_string((TValue[3]){ error_message, target, name }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    MethodDescriptor* desc = as_method_descriptor(method);
    TTable* args = AS_TABLE(desc->arg_types);

    check_call_types(node, args, context);
    return desc->return_type;
}


TypesContext* new_resolve_context() {
    TypesContext* context = malloc(sizeof(TypesContext));
    context->current_class = NIL;
    context->return_type = varType;
    context->type_namespace = AS_TABLE(new_table(NULL, 0));
    context->id_namespace = AS_TABLE(new_table(NULL, 0));
    context->func_namespace = AS_TABLE(new_table(NULL, 0));

    TValue num_desc = make_class_descriptor(num_map);
    TValue str_desc = make_class_descriptor(str_map);
    TValue tbl_desc = make_class_descriptor(table_map);

    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), numType, num_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), strType, str_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), tableType, tbl_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), NumberType, num_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), StringType, str_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), TableType, tbl_desc);
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), funcType, make_class_descriptor(func_map));

    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), NumberType, dunder(NumberType));
    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), StringType, dunder(StringType));
    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), TableType, dunder(TableType));
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), dunder(NumberType), make_class_descriptor(NIL));
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), dunder(StringType), make_class_descriptor(NIL));
    add_value_to_hashtable(TBL_TO_TVALUE(context->type_namespace), dunder(TableType), make_class_descriptor(NIL));

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

TValue check_call_types(Node* call_node, TTable* expecteds, TypesContext* context) {
    if (expecteds->array.len < call_node->node.CallNode.args.len) call_error(new_invalid_args_error, "Called function with too many args");
    for (int i = 0; i < expecteds->array.len; i++) {
        Node* cur_arg = call_node->node.CallNode.args.nodes[i];
        TValue cur_type = resolve_type(cur_arg, context);
        TValue expected_arg = expecteds->array.array[expecteds->array.len - i - 1];

        if (!EQUAL(expected_arg, cur_type) && !EQUAL(expected_arg, varType) && !EQUAL(cur_type, varType)) {
            TValue error_message = make_fstring("Passed argument of type <%s> when type <%s> was expected, in function <%s>");
            error_message = format_string((TValue[4]){ error_message, cur_type, expected_arg, call_node->node.CallNode.target->node.VarAcess.name }, 4);
            
            call_error(new_type_error, AS_STRING(error_message)->str);
        }
    }
}

TValue dunder(TValue str) {
    TValue dunder_str = make_fstring("__");
    return concat_string((TValue[2]){ dunder_str, str }, 2);
}

TValue undunder(TValue str) {
    return make_fstring(AS_STRING(str)->str + 2);
}
