#include "TypeResolving/type_resolving.h"
#include "Values/namespaces.h"
#include "Globals/string.h"
#include "Globals/table.h"
#include "Globals/tError.h"
#include "Globals/Module/module.h"


TValue type_namespace;
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
    [NT_TABLE]          = resolve_type_table,
    [NT_ALLOCATION]     = resolve_type_allocation,
    [NT_IF]             = resolve_type_if_statement,
    [NT_NFOR]           = resolve_type_nfor_loop,
    [NT_GFOR]           = resolve_type_gfor_loop,
    [NT_WHILE]          = resolve_type_while_loop,
    [NT_REPEAT]         = resolve_type_repeat_loop,
    [NT_REQUIRE]        = resolve_type_require,
    [NT_IMPORT]         = resolve_type_import,
    [NT_FROM_IMPORT]    = resolve_type_from_import,
    [NT_FUNCDEF]        = resolve_type_func_def,
    [NT_METHODDEF]      = resolve_type_method_def,
    [NT_CLASSDEF]       = resolve_type_class_def,
    [NT_SELFCALL]       = resolve_type_selfcall,
    [NT_SELFDECLARE]    = resolve_type_selfdeclare,
    [NT_FIELDACCESS]    = resolve_type_fieldaccess,
    [NT_CONSTRUCTOR]    = resolve_type_constructor,
    [NT_INDEXING]       = resolve_type_indexing
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
    TValue var_type = node->node.VarDeclareNode.type_specifier;

    if (node->node.VarDeclareNode.value) {
        TValue return_type = resolve_type(node->node.VarDeclareNode.value, context);
        if (!EQUAL(var_type, varType) && !EQUAL(return_type, varType) && !EQUAL(var_type, return_type)) {
            TValue error_message = make_fstring("Cannot assign value of type <%s> to variable of type <%s>");
            error_message = format_string((TValue[3]){ error_message, return_type, var_type }, 3);

            call_error(new_type_error, AS_STRING(error_message)->str);
        }
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
    TValue lcdesc = get_value_from_hashtable(type_namespace, ltype);
    TValue rcdesc = get_value_from_hashtable(type_namespace, rtype);
    TValue lmap = TBL_TO_TVALUE(as_class_descriptor(lcdesc)->func_namespace);
    TValue rmap = TBL_TO_TVALUE(as_class_descriptor(rcdesc)->func_namespace);
    TValue ldesc = get_value_from_hashtable(lmap, op);
    TValue rdesc = get_value_from_hashtable(rmap, rop);
    MethodDescriptor* ldescPtr = IS_NIL(ldesc) ? NULL : (MethodDescriptor*)PAYLOAD(ldesc);
    MethodDescriptor* rdescPtr = IS_NIL(rdesc) ? NULL : (MethodDescriptor*)PAYLOAD(rdesc);

    if (ldescPtr && in_table(ldescPtr->arg_types, rtype)) return (node->type = ldescPtr->return_type);
    if (rdescPtr && in_table(rdescPtr->arg_types, ltype)) return (node->type = rdescPtr->return_type);

    TValue error_message = make_fstring("Cannot proceed to operation : %s with types : <%s> and <%s>");
    error_message = format_string((TValue[4]){ error_message, op, ltype, rtype }, 4);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}

TValue resolve_type_uni_op(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.UniOpNode.target, context);
    if (IS_VAR(return_type)) return varType;

    TValue op = get_value_from_hashtable(unary_operation_methods, NUM_AS_TVALUE(node->node.UniOpNode.op.tt));
    TValue cdesc = get_value_from_hashtable(type_namespace, return_type);
    TValue map = TBL_TO_TVALUE(as_class_descriptor(cdesc)->id_namespace);
    TValue desc = get_value_from_hashtable(map, op);
    MethodDescriptor* descPtr = IS_NIL(desc) ? NULL : (MethodDescriptor*)PAYLOAD(desc);

    if (descPtr && in_table(descPtr->arg_types, return_type)) return (node->type = descPtr->return_type);
    else if (node->node.UniOpNode.op.tt == TT_NOT) return (node->type = NumberType);

    TValue error_message = make_fstring("Cannot proceed to unary operation : %s with type : <%s>");
    error_message = format_string((TValue[3]){ error_message, op, return_type }, 3);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}

TValue resolve_type_call(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.CallNode.target, context);

    if (!EQUAL(target_type, FunctionType) && !EQUAL(target_type, varType)) {
        TValue error_message = make_fstring("Cannot call value of type <%s>");
        error_message = format_string((TValue[3]){ error_message, target_type }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    if (node->node.CallNode.target->nt == NT_VAR_ACESS) {
        TValue target_desc = get_value_from_hashtable(TBL_TO_TVALUE(context->func_namespace), node->node.CallNode.target->node.VarAcess.name);
        target_desc = IS_NIL(target_desc) ? get_value_from_hashtable(global_functypes, node->node.CallNode.target->node.VarAcess.name) : target_desc;

        if (!IS_NIL(target_desc)) {
            MethodDescriptor* desc = as_method_descriptor(target_desc);
            TTable* args = AS_TABLE(desc->arg_types);

            check_call_types(node, args, context, node->node.CallNode.target->node.VarAcess.name);
            return (node->type = desc->return_type);
        }
    }

    return (node->type = varType);
}

TValue resolve_type_var_access(Node* node, TypesContext* context) {
    TValue var_type = get_symbol_type(node->node.VarAcess.name, context);
    var_type = IS_NIL(var_type) ? get_value_from_hashtable(global_types, node->node.VarAcess.name) : var_type;

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

TValue resolve_type_table(Node* node, TypesContext* context) {
    for (int i = 0; i < node->node.TableNode.values.len; i++) {
        resolve_type(node->node.TableNode.values.nodes[i], context);
    }

    return (node->type = TableType);
}

TValue resolve_type_allocation(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.AllocNode.target, context);
    for (int i = 0; i < node->node.AllocNode.args.len; i++) {
        resolve_type(node->node.AllocNode.args.nodes[i], context);
    }

    if (EQUAL(target_type, varType) || EQUAL(target_type, TableType)) return (node->type = tableType);
    call_error(new_type_error, "Allocation only work on type <Table>");
}

TValue resolve_type_if_statement(Node* node, TypesContext* context) {
    for (int i = 0; i < node->node.IfNode.cases.len; i++) {
        resolve_type(node->node.IfNode.cases.nodes[i], context);
    }

    for (int i = 0; i < node->node.IfNode.statements.len; i++) {
        resolve_in_scope(node->node.IfNode.statements.nodes[i], context);
    }

    return (node->type = varType);
}

TValue resolve_type_nfor_loop(Node* node, TypesContext* context) {
    TValue v1 = resolve_type(node->node.nForNode.values[0], context),
    v2 = resolve_type(node->node.nForNode.values[1], context),
    v3 = resolve_type(node->node.nForNode.values[2], context);

    int v1cond = EQUAL(v1, varType) || EQUAL(v1, NumberType);
    int v2cond = EQUAL(v2, varType) || EQUAL(v2, NumberType);
    int v3cond = EQUAL(v3, varType) || EQUAL(v3, NumberType);

    if (!v1cond || !v2cond || !v3cond) {
        TValue error_message = make_fstring("numeric for loops can only use type <Number>");
        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    open_type_scope(context);
    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.nForNode.identifier, NumberType);
    resolve_type(node->node.nForNode.statements, context);
    close_type_scope(context);

    return (node->type = varType);
}

TValue resolve_type_gfor_loop(Node* node, TypesContext* context) {
    TValue target_type = resolve_type(node->node.gForNode.target, context);
    TValue v = node->node.gForNode.identifiers[1];
    TValue i_type = varType;
    TValue v_type = varType;

    if (!EQUAL(target_type, varType)) {
        TValue target_cdesc = get_value_from_hashtable(type_namespace, target_type);
        TValue target_map = TBL_TO_TVALUE(as_class_descriptor(target_cdesc)->func_namespace);
        TValue iter_desc = get_value_from_hashtable(target_map, make_fstring("__iter"));

        if (IS_NIL(iter_desc)) {
            TValue error_message = make_fstring("Type <%s> do not have an __iter method");
            error_message = format_string((TValue[2]){ error_message, target_type }, 2);
            
            call_error(new_type_error, AS_STRING(error_message)->str);
        }

        TTable* arg_types = AS_TABLE(as_method_descriptor(iter_desc)->arg_types);
        if (arg_types->array.len < 2) {
            TValue error_message = make_fstring("__iter method of type <%s> must have 2 arguments");
            error_message = format_string((TValue[2]){ error_message, target_type }, 2);
            
            call_error(new_type_error, AS_STRING(error_message)->str);
        }

        i_type = arg_types->array.array[0];
        v_type = arg_types->array.array[1];
    }

    open_type_scope(context);
    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.gForNode.identifiers[0], i_type);
    if (!IS_NIL(v)) add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), v, v_type);
    resolve_type(node->node.gForNode.statements, context);
    close_type_scope(context);

    return (node->type = varType);
}

TValue resolve_type_while_loop(Node* node, TypesContext* context) {
    resolve_type(node->node.whileNode.condition, context);
    resolve_in_scope(node->node.whileNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_repeat_loop(Node* node, TypesContext* context) {
    resolve_type(node->node.repeatNode.condition, context);
    resolve_in_scope(node->node.repeatNode.statements, context);
    return (node->type = varType);
}

TValue resolve_type_require(Node* node, TypesContext* context) {
    TValue type = node->node.importNode.module;
    type = concat_string((TValue[2]){ make_fstring("Module "), type }, 2);

    TValue cached = get_value_from_hashtable(type_namespace, type);
    if (IS_NIL(cached)) {
        TypesContext* import_context = new_resolve_context();
        resolve_type(node->node.importNode.ast, import_context);

        TValue func_namespace = TBL_TO_TVALUE(import_context->func_namespace);
        TValue id_namespace = TBL_TO_TVALUE(import_context->id_namespace);
        func_namespace = copy_table(&func_namespace, 1);
        id_namespace = copy_table(&id_namespace, 1);

        add_value_to_hashtable(type_namespace, type, make_class_descriptor(func_namespace, id_namespace));
        close_resolve_context(import_context);
    }

    if (!IS_NIL(node->node.importNode.identifier)) add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.importNode.identifier, node->node.importNode.type_specifier);
    return node->type = node->node.importNode.type_specifier;
}

TValue resolve_type_import(Node* node, TypesContext* context) {
    TValue type = node->node.importNode.module;
    type = concat_string((TValue[2]){ make_fstring("Module "), type }, 2);

    TValue cached = get_value_from_hashtable(type_namespace, type);
    if (IS_NIL(cached)) {
        TypesContext* import_context = new_resolve_context();
        resolve_type(node->node.importNode.ast, import_context);

        TValue func_namespace = TBL_TO_TVALUE(import_context->func_namespace);
        TValue id_namespace = TBL_TO_TVALUE(import_context->id_namespace);
        func_namespace = copy_table(&func_namespace, 1);
        id_namespace = copy_table(&id_namespace, 1);

        add_value_to_hashtable(type_namespace, type, make_class_descriptor(func_namespace, id_namespace));
        close_resolve_context(import_context);
    }

    if (!IS_NIL(node->node.importNode.identifier)) add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), node->node.importNode.identifier, type);
    return node->type = type;
}

TValue resolve_type_from_import(Node* node, TypesContext* context) {
    Node import_node = {
        .nt = NT_IMPORT,
        .node.importNode.ast = node->node.fromImportNode.ast,
        .node.importNode.identifier = NIL,
        .node.importNode.type_specifier = NIL,
        .node.importNode.module = node->node.fromImportNode.module
    };

    resolve_type(&import_node, context);

    TValue path = node->node.importNode.module;
    TValue module = concat_string((TValue[2]){ make_fstring("Module "), path }, 2);
    TValue desc = get_value_from_hashtable(type_namespace, module);
    TValue map = TBL_TO_TVALUE(as_class_descriptor(desc)->id_namespace);

    int argc = get_linked_list_len(node->node.fromImportNode.elements);
    TValue elements[argc];      linked_list_to_array(node->node.fromImportNode.elements, elements, argc);
    TValue elem_types[argc];    linked_list_to_array(node->node.fromImportNode.elem_types, elem_types, argc);
    TValue nicknames[argc];     linked_list_to_array(node->node.fromImportNode.nicknames, nicknames, argc);

    for (int i = 0; i < argc; i++) {
        TValue elem_real_type = get_value_from_hashtable(map, elements[i]);
        if (IS_NIL(elem_real_type)) {
            TValue error_message = make_fstring("Module <%s> does not have any variable of name <%s>");
            error_message = format_string((TValue[3]){ error_message, module, elem_real_type }, 3);

            call_error(new_type_error, AS_STRING(error_message)->str);
        }

        if (!EQUAL(elem_real_type, varType) && !EQUAL(elem_types[i], varType) && !EQUAL(elem_real_type, elem_types[i])) {
            TValue error_message = make_fstring("cannot assign value of type <%s> to variable of type <%s>. from Module <%s>");
            error_message = format_string((TValue[4]){ error_message, elem_types[i], elem_real_type, module }, 4);

            call_error(new_type_error, AS_STRING(error_message)->str);
        }

        TValue name = IS_NIL(nicknames[i]) ? elements[i] : nicknames[i];
        add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), name, elem_types[i]);
    }

    return varType;
}

TValue resolve_type_func_def(Node* node, TypesContext* context) {
    TValue pre_return_type = context->return_type;
    TValue pre_class = context->current_class;
    TTable* id_stack = context->id_stack;
    TTable* func_stack = context->func_stack;

    context->id_stack = AS_TABLE(new_table(NULL, 0));
    context->func_stack = AS_TABLE(new_table(NULL, 0));
    context->return_type = node->node.funcDefNode.type_specifier;

    open_type_scope(context);

    int argc = get_linked_list_len(node->node.funcDefNode.arg_types);
    TValue arg_array[argc]; linked_list_to_array(node->node.funcDefNode.args, arg_array, argc);
    TValue argtype_array[argc]; linked_list_to_array(node->node.funcDefNode.arg_types, argtype_array, argc);

    for (int i = 0; i < argc; i++) add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), arg_array[i], argtype_array[i]);

    resolve_type(node->node.funcDefNode.statements, context);
    close_type_scope(context);

    context->id_stack = id_stack;
    context->func_stack = func_stack;

    open_type_scope(context);
    close_type_scope(context);

    context->return_type = pre_return_type;
    context->current_class = pre_class;

    if (!IS_NIL(node->node.funcDefNode.name)) {
        TValue name = node->node.funcDefNode.name;

        TValue desc = make_method_descriptor(argtype_array, node->node.funcDefNode.type_specifier, argc);
        add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), name, FunctionType);
        add_value_to_hashtable(TBL_TO_TVALUE(context->func_namespace), name, desc);
    }

    return (node->type = FunctionType);
}

TValue resolve_type_method_def(Node* node, TypesContext* context) {
    TValue pre_return_type = context->return_type;
    TValue pre_class = context->current_class;
    TTable* id_stack = context->id_stack;
    TTable* func_stack = context->func_stack;

    context->id_stack = AS_TABLE(new_table(NULL, 0));
    context->func_stack = AS_TABLE(new_table(NULL, 0));
    context->return_type = node->node.funcDefNode.type_specifier;

    open_type_scope(context);

    context->current_class = node->node.methodDefNode.class;
    add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), make_fstring("self"), node->node.methodDefNode.class);

    int argc = get_linked_list_len(node->node.funcDefNode.arg_types);
    TValue arg_array[argc]; linked_list_to_array(node->node.funcDefNode.args, arg_array, argc);
    TValue argtype_array[argc]; linked_list_to_array(node->node.funcDefNode.arg_types, argtype_array, argc);

    for (int i = 0; i < argc; i++) add_value_to_hashtable(TBL_TO_TVALUE(context->id_namespace), arg_array[i], argtype_array[i]);

    resolve_type(node->node.funcDefNode.statements, context);
    close_type_scope(context);

    context->id_stack = id_stack;
    context->func_stack = func_stack;
    context->return_type = pre_return_type;
    context->current_class = pre_class;

    if (!IS_NIL(node->node.funcDefNode.name)) {
        TValue name = node->node.funcDefNode.name;

        TValue desc = make_method_descriptor(argtype_array, node->node.funcDefNode.type_specifier, argc);
        TValue target_class = has_modifier(node->node.methodDefNode.modifiers, "static") ? context->current_class : node->node.methodDefNode.class;
        node->node.methodDefNode.class = context->current_class;

        TValue class_desc = get_value_from_hashtable(type_namespace, target_class);
        TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->id_namespace);
        TValue class_func = TBL_TO_TVALUE(as_class_descriptor(class_desc)->func_namespace);

        if (in_table(native_methods, dunder(name))) {
            TValue error_message = make_fstring("Cannot name a method <%s>");
            error_message = format_string((TValue[2]){ error_message, name }, 2);

            call_error(new_invalid_identifier_error, AS_STRING(error_message)->str);
        }
        
        add_value_to_hashtable(class_map, name, FunctionType);
        add_value_to_hashtable(class_func, name, desc);

    } else {
        TValue error_message = make_fstring("Methods must have names");
        error_message = format_string((TValue[2]){ error_message }, 2);

        call_error(new_invalid_identifier_error, AS_STRING(error_message)->str);
    }

    return (node->type = FunctionType);
}

TValue resolve_type_class_def(Node* node, TypesContext* context) {
    TStr* name = AS_STRING(node->node.classDefNode.name);
    TValue pre_class = context->current_class;

    context->current_class = node->node.classDefNode.name;
    if (is_dunder(STR_TO_TVALUE(name)).num) {
        TValue error_message = make_fstring("Invalid class name <%s>, class name cannot start with '__'");
        error_message = format_string((TValue[2]){ error_message, context->current_class }, 2);

        call_error(new_invalid_identifier_error, AS_STRING(error_message)->str);
    }

    TValue dunder_class = dunder(context->current_class);

    add_value_to_hashtable(global_types, context->current_class, dunder_class);
    add_value_to_hashtable(type_namespace, dunder_class, make_class_descriptor(NIL, NIL));
    add_value_to_hashtable(type_namespace, context->current_class, make_class_descriptor(NIL, NIL));

    open_type_scope(context);

    context->current_class = dunder_class;
    for (int i = 0; i < node->node.classDefNode.statements->node.StatementsNode.len; i++) {
        Node* cur = node->node.classDefNode.statements->node.StatementsNode.nodes[i];
        if (cur->nt == NT_FUNCDEF) {
            if (EQUAL(cur->node.funcDefNode.name, constructor)) cur->nt = NT_CONSTRUCTOR;
            else cur->nt = NT_METHODDEF;
            
            cur->node.methodDefNode.class = node->node.classDefNode.name;
        }

        resolve_type(cur, context);
    }

    close_type_scope(context);
    context->current_class = pre_class;
    return (node->type = dunder_class);
}

TValue resolve_type_selfcall(Node* node, TypesContext* context) {
    TValue target = resolve_type(node->node.SelfCallnode.target, context);

    TValue name = node->node.SelfCallnode.name;
    TValue dundered = get_value_from_hashtable(native_methods_map, name);
    name = IS_NIL(dundered) ? name : dundered;

    node->node.SelfCallnode.name = name;

    if (EQUAL(target, varType)) call_error(new_type_error, "Must specify type of value in order to self-call");
    TValue class_desc = get_value_from_hashtable(type_namespace, target);
    TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->func_namespace);

    if (IS_NIL(class_map)) {
        TValue error_message = make_fstring("Type <%s> does not have a proper method map");
        error_message = format_string((TValue[2]){ error_message, target }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue method = get_value_from_hashtable(class_map, name);

    if (IS_NIL(method)) {
        TValue error_message = make_fstring("Type <%s> does not have any method of name <%s>");
        error_message = format_string((TValue[3]){ error_message, target, name }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    MethodDescriptor* desc = as_method_descriptor(method);
    TTable* args = AS_TABLE(desc->arg_types);

    TValue full_method = format_string((TValue[3]){ make_fstring("%s.%s"), target, name }, 3);
    check_call_types(node, args, context, full_method);

    return (node->type = desc->return_type);
}

TValue resolve_type_selfdeclare(Node* node, TypesContext* context) {
    TValue return_type = resolve_type(node->node.VarDeclareNode.value, context);
    TValue var_type = node->node.VarDeclareNode.type_specifier;

    if (!EQUAL(var_type, varType) && !EQUAL(return_type, varType) && !EQUAL(var_type, return_type)) {
        TValue error_message = make_fstring("Cannot assign value of type <%s> to attribute of type <%s>");
        error_message = format_string((TValue[3]){ error_message, return_type, var_type }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue name = node->node.SelfDeclarenode.var_name;
    TValue class_desc = get_value_from_hashtable(type_namespace, context->current_class);
    TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->id_namespace);

    if (in_table(native_methods, dunder(name))) {
        TValue error_message = make_fstring("Cannot name a attribute <%s>");
        error_message = format_string((TValue[2]){ error_message, name }, 2);

        call_error(new_invalid_identifier_error, AS_STRING(error_message)->str);
    }
    
    node->node.SelfDeclarenode.class = context->current_class;
    add_value_to_hashtable(class_map, name, return_type);
    return (node->type = var_type);
}

TValue resolve_type_fieldaccess(Node* node, TypesContext* context) {
    TValue target = resolve_type(node->node.FieldAccessNode.target, context);
    if (EQUAL(target, varType)) call_error(new_type_error, "Must specify type of value in order to access a field");

    TValue name = node->node.FieldAccessNode.name;
    TValue dundered = get_value_from_hashtable(native_methods_map, name);
    name = IS_NIL(dundered) ? name : dundered;

    node->node.FieldAccessNode.name = name;

    TValue class_desc = get_value_from_hashtable(type_namespace, target);
    if (IS_NIL(class_desc)) {
        TValue error_message = make_fstring("Type <%s> does not exist");
        error_message = format_string((TValue[2]){ error_message, target }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->id_namespace);

    if (IS_NIL(class_map)) {
        TValue error_message = make_fstring("Type <%s> does not have a proper method map");
        error_message = format_string((TValue[2]){ error_message, target }, 2);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue field = get_value_from_hashtable(class_map, name);
    if (IS_NIL(field)) {
        TValue error_message = make_fstring("Type <%s> does not have any field of name <%s>");
        error_message = format_string((TValue[3]){ error_message, target, name }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    return (node->type = field);
}

TValue resolve_type_constructor(Node* node, TypesContext* context) {
    TValue modifiers = node->node.methodDefNode.modifiers;
    TValue func_type = node->node.methodDefNode.type_specifier;
    TValue class = node->node.methodDefNode.class;
    if (!has_modifier(modifiers, "static")) call_error(new_type_error, "A class's constructor ('__new') must be static");
    if (!EQUAL(func_type, varType) && !EQUAL(func_type, class)) {
        TValue error_message = make_fstring("A class's constructor ('__new') must be return the object\n\tTried returning type <%s> in method of class <%s>");
        error_message = format_string((TValue[3]){ error_message, func_type, class }, 3);

        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    return resolve_type_method_def(node, context);
}

TValue resolve_type_indexing(Node* node, TypesContext* context) {
    TValue target = resolve_type(node->node.IndexNode.target, context);
    TValue index = resolve_type(node->node.IndexNode.index, context);

    if (EQUAL(target, varType)) return varType;
    TValue class_desc = get_value_from_hashtable(type_namespace, target);
    TValue class_map = TBL_TO_TVALUE(as_class_descriptor(class_desc)->func_namespace);

    TValue desc = get_value_from_hashtable(class_map, indexer);
    MethodDescriptor* descPtr = IS_NIL(desc) ? NULL : (MethodDescriptor*)PAYLOAD(desc);

    if (!descPtr) {
        TValue error_message = make_fstring("Cannot index type : <%s>");
        error_message = format_string((TValue[4]){ error_message, target }, 2);
        
        call_error(new_type_error, AS_STRING(error_message)->str);
    }

    TValue expected_type = get_from_table(descPtr->arg_types, NUM_AS_TVALUE(0));
    if (EQUAL(index, varType) || EQUAL(expected_type, varType) || EQUAL(expected_type, index)) return descPtr->return_type;

    TValue error_message = make_fstring("Cannot index type : <%s> with type <%s>");
    error_message = format_string((TValue[4]){ error_message, target, index }, 3);
    
    call_error(new_type_error, AS_STRING(error_message)->str);
}


TypesContext* new_resolve_context() {
    TypesContext* context = malloc(sizeof(TypesContext));
    context->current_class = NIL;
    context->return_type = varType;
    context->id_stack = AS_TABLE(new_table(NULL, 0));
    context->func_stack = AS_TABLE(new_table(NULL, 0));

    open_type_scope(context);
    return context;
}

TValue check_call_types(Node* call_node, TTable* expecteds, TypesContext* context, TValue func_name) {
    if (expecteds->array.len < call_node->node.CallNode.args.len) call_error(new_invalid_args_error, "Called function with too many args");
    for (int i = 0; i < call_node->node.CallNode.args.len; i++) {
        Node* cur_arg = call_node->node.CallNode.args.nodes[i];
        TValue cur_type = resolve_type(cur_arg, context);
        TValue expected_arg = expecteds->array.array[i];

        if (!EQUAL(expected_arg, cur_type) && !EQUAL(expected_arg, varType) && !EQUAL(cur_type, varType)) {
            TValue error_message = make_fstring("Passed argument of type <%s> when type <%s> was expected, in function <%s>");
            error_message = format_string((TValue[4]){ error_message, cur_type, expected_arg, func_name }, 4);
            
            call_error(new_type_error, AS_STRING(error_message)->str);
        }
    }
}

static TValue get_symbol_type(TValue name, TypesContext* context) {
    for (int i = context->id_stack->array.len - 1; i >= 0; i--) {
        TValue id_table = context->id_stack->array.array[i];
        TValue tidx = get_value_from_hashtable(id_table, name);
        if (!IS_NIL(tidx)) return tidx;
    }

    return NIL;
}

static TValue get_func_descriptor(TValue name, TypesContext* context) {
    for (int i = context->func_stack->array.len - 1; i >= 0; i--) {
        TValue func_table = context->func_stack->array.array[i];
        TValue tidx = get_value_from_hashtable(func_table, name);
        if (!IS_NIL(tidx)) return tidx;
    }

    return NIL;
}

static inline TValue resolve_in_scope(Node* node, TypesContext* context) {
    open_type_scope(context);
    TValue res = resolve_type(node, context);
    close_type_scope(context);

    return res;
}

static void open_type_scope(TypesContext* context) {
    append_to_table(TBL_TO_TVALUE(context->id_stack), new_table(NULL, 0));
    append_to_table(TBL_TO_TVALUE(context->func_stack), new_table(NULL, 0));
    update_types(context);
}

static void close_type_scope(TypesContext* context) {
    TValue last_id = get_from_table(TBL_TO_TVALUE(context->id_stack), NUM_AS_TVALUE(context->id_stack->array.len - 1));
    TValue last_func = get_from_table(TBL_TO_TVALUE(context->func_stack), NUM_AS_TVALUE(context->func_stack->array.len - 1));

    free_table(&last_id, 1);
    free_table(&last_func, 1);

    remove_from_table(TBL_TO_TVALUE(context->id_stack), NUM_AS_TVALUE(context->id_stack->array.len - 1));
    remove_from_table(TBL_TO_TVALUE(context->func_stack), NUM_AS_TVALUE(context->func_stack->array.len - 1));
    update_types(context);
}

static inline void update_types(TypesContext* context) {
    if (!context->id_stack->array.len) return;
    context->id_namespace = AS_TABLE(get_from_table(TBL_TO_TVALUE(context->id_stack), NUM_AS_TVALUE(context->id_stack->array.len - 1)));
    context->func_namespace = AS_TABLE(get_from_table(TBL_TO_TVALUE(context->func_stack), NUM_AS_TVALUE(context->func_stack->array.len - 1)));
}

void close_resolve_context(TypesContext* context) {
    close_type_scope(context);
    free_table((TValue[1]){ TBL_TO_TVALUE(context->id_stack) }, 1);
    free_table((TValue[1]){ TBL_TO_TVALUE(context->func_stack) }, 1);
    free(context);
}