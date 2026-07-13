#include <stdlib.h>
#include <math.h>

#include "Compiler/compiler.h"
#include "TypeResolving/type_specifiers.h"
#include "Globals/Module/module.h"
#include "bytescript.h"
#include "labeler.h"

#define false 0
#define true 1

CompileFunction compiler_functions[] = {
    [NT_STATEMENTS]     = compile_statements,
    [NT_RETURN]         = compile_return,
    [NT_BREAK]          = compile_break,
    [NT_CONTINUE]       = compile_continue,
    [NT_VAR_DECLARE]    = compile_var_declare,
    [NT_ASSIGNEMENT]    = compile_assignement,
    [NT_BIN_OP]         = compile_bin_op,
    [NT_UNI_OP]         = compile_uni_op,
    [NT_CALL]           = compile_call,
    [NT_VAR_ACESS]      = compile_var_access,
    [NT_NUMBER]         = compile_number,
    [NT_STRING]         = compile_string,
    [NT_TABLE]          = compile_table,
    [NT_ALLOCATION]     = compile_allocation,
    [NT_IF]             = compile_if_statement,
    [NT_NFOR]           = compile_nfor_loop,
    [NT_GFOR]           = compile_gfor_loop,
    [NT_WHILE]          = compile_while_loop,
    [NT_REPEAT]         = compile_repeat_loop,
    [NT_REQUIRE]        = compile_require,
    [NT_IMPORT]         = compile_import,
    [NT_FROM_IMPORT]    = compile_from_import,
    [NT_FUNCDEF]        = compile_func_def,
    [NT_METHODDEF]      = compile_method_def,
    [NT_CLASSDEF]       = compile_class_def,
    [NT_SELFCALL]       = compile_selfcall,
    [NT_SELFDECLARE]    = compile_selfdeclare,
    [NT_FIELDACCESS]    = compile_fieldaccess,
    [NT_CONSTRUCTOR]    = compile_constructor,
    [NT_INDEXING]       = compile_indexing
};


int compile(Node* node, Scope* context) { return compiler_functions[node->nt](node, context); }

int compile_statements(Node* node, Scope* context) {
    int res = 0;
    
    for (int i = 0; i < node->node.StatementsNode.len; i++) {
        Node* compile_node = node->node.StatementsNode.nodes[i];
        res = compile(compile_node, context);
    }

    return res;
}

int compile_return(Node* node, Scope* context) {
    int r0 = free_register(register_top(context), context);
    if (node->node.ReturnNode.len == 1) {
        Node* nd = node->node.ReturnNode.nodes[0];
        int return_v = compile(nd, context);

        if (nd->cc) r0 = register_constant(return_v, context, -1); else r0 = register_specific(return_v, context);

    } else if (node->node.ReturnNode.len > 1) {
        for (int i = 0; i < node->node.ReturnNode.len; i++) register_top(context);
        for (int i = 0; i < node->node.ReturnNode.len; i++) {
            Node* nd = node->node.ReturnNode.nodes[i];
            int return_v = compile(nd, context);

            if (nd->cc) register_constant(return_v, context, r0 + i); else register_register(return_v, context, r0 + i);
        }
    }

    emit(OP_RETURN, context);
    emit(free_multiple(r0, node->node.ReturnNode.len, context), context);
    emit(node->node.ReturnNode.len, context);
    emit(0x00, context);
    return r0;
}

int compile_break(Node* node, Scope* context) {
    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    struct sizeLinkedList* new = malloc(sizeof(struct sizeLinkedList));
    new->size = CURRENT_INSTR;
    new->next = context->breaks.next;

    context->breaks.next = new;
    return 0;
}

int compile_continue(Node* node, Scope* context) {
    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    struct sizeLinkedList* new = malloc(sizeof(struct sizeLinkedList));
    new->size = CURRENT_INSTR;
    new->next = context->continues.next;

    context->continues.next = new;
    return 0;
}

int compile_var_declare(Node* node, Scope* context) {
    if (node->node.VarDeclareNode.value) {
        int r = -1;
        int value = compile(node->node.VarDeclareNode.value, context);

        if (node->node.VarDeclareNode.value->cc) r = register_constant(value, context, -1); else r = register_register(value, context, -1);
        append_symbol(context, node->node.VarDeclareNode.var_name, r);

        return r;
    }

    int r = register_first(context);
    append_symbol(context, node->node.VarDeclareNode.var_name, r);

    return r;
}

int compile_assignement(Node* node, Scope* context) {
    Node* lvalue_node = node->node.AssignementNode.lvalue;
    Node* value_node = node->node.AssignementNode.value;

    switch (lvalue_node->nt) {
        case NT_VAR_ACESS: {
            int lvalue = compile(lvalue_node, context);

            int analysis_res = assignement_analysis(node);
            if (analysis_res) free_register(lvalue, context);

            int value = compile(value_node, context);
            if (value_node->cc) value = register_constant(value, context, lvalue);

            return register_register(value, context, lvalue);
        }

        case NT_INDEXING: {
            Node* target_node = lvalue_node->node.IndexNode.target;
            Node* index_node = lvalue_node->node.IndexNode.index;

            int target = compile(target_node, context);
            int free_target = target_node->cc ? 1 : register_try_free(target, context) + 1;

            int index = compile(index_node, context);
            int free_index = index_node->cc ? 1 : register_try_free(index, context) + 1;

            int value = compile(node->node.AssignementNode.value, context);

            if (target_node->cc) target = register_constant(target, context, -1);
            if (index_node->cc) index = register_constant(index, context, -1);
            if (value_node->cc) value = free_register(register_constant(value, context, -1), context);

            if (free_target) free_register(target, context);
            if (free_index) free_register(index, context);

            emit(OP_SETTABLE, context);
            emit(target, context);
            emit(value, context);
            emit(index, context);

            return value;
        }

        default : call_error(new_incorrect_lvalue_error, "cannot assign value to rvalue");
    }
}

int compile_bin_op(Node* node, Scope* context) {
    int left = compile(node->node.BinOpNode.left, context);
    bool leftCC = node->node.BinOpNode.left->cc;

    int free_left = leftCC ? 0 : register_try_free(left, context) + 1;

    int right = compile(node->node.BinOpNode.right, context);
    bool rightCC = node->node.BinOpNode.right->cc;

    int op = node->node.BinOpNode.op.tt;
    if (free_left) free_register(left, context);

    bool CC = leftCC || rightCC;
    switch (op) {
        case TT_MINUS:  emit(leftCC ? OP_SUB_CR    : rightCC ? OP_SUB_RC     : OP_SUB_RR, context); break;
        case TT_DIV:    emit(leftCC ? OP_DIV_CR    : rightCC ? OP_DIV_RC     : OP_DIV_RR, context); break;
        case TT_IDIV:   emit(leftCC ? OP_IDIV_CR   : rightCC ? OP_IDIV_RC    : OP_IDIV_RR, context); break;
        case TT_MOD:    emit(leftCC ? OP_MOD_CR    : rightCC ? OP_MOD_RC     : OP_MOD_RR, context); break;
        case TT_POW:    emit(leftCC ? OP_POW_CR    : rightCC ? OP_POW_RC     : OP_POW_RR, context); break;
        case TT_RAND:
        case TT_ROR:
        case TT_AND:
        case TT_OR:     compile_boolean_op(left, right, leftCC, rightCC, op, context);

        default :
            if (leftCC) { int temp = left; left = right; right = temp; }

            switch (op) {
                case TT_PLUS:   emit(CC ? OP_ADD_RC  : OP_ADD_RR, context); break;
                case TT_MUL:    emit(CC ? OP_MUL_RC  : OP_MUL_RR, context); break;
                case TT_BAND:   emit(CC ? OP_BAND_RC : OP_BAND_RR, context); break;
                case TT_BOR:    emit(CC ? OP_BOR_RC  : OP_BOR_RR, context); break;
                case TT_BXOR:   emit(CC ? OP_BXOR_RC : OP_BXOR_RR, context); break;
                case TT_EEQ:    emit(CC ? OP_EEQ_RC  : OP_EEQ_RR, context); break;
                case TT_GT:     emit(leftCC ? OP_LT_RC : rightCC ? OP_GT_RC : OP_GT_RR, context); break;
                case TT_LT:     emit(leftCC ? OP_GT_RC : rightCC ? OP_LT_RC : OP_GT_RR, context); break;
                case TT_GTEQ:   emit(leftCC ? OP_LTEQ_RC : rightCC ? OP_GTEQ_RC : OP_GTEQ_RR, context); break;
                case TT_LTEQ:   emit(leftCC ? OP_GTEQ_RC : rightCC ? OP_LTEQ_RC : OP_LTEQ_RR, context); break;
            }
    }

    int r = free_register(register_first(context), context);
    emit(r, context);
    emit(left, context);
    emit(right, context);

    return r;
}

int compile_uni_op(Node* node, Scope* context) {
    int r = 0;
    int target = compile(node->node.UniOpNode.target, context);
    node->cc = node->node.UniOpNode.target->cc;

    switch (node->node.UniOpNode.op.tt) {
        case TT_PLUS: return target;
        case TT_MINUS: {
            if (node->cc) {
                TValue v = get_constant(target, context);
                return cache_constant(NUM_AS_TVALUE(-AS_NUM(v)), context);
            }

            r = free_register(register_first(context), context);
            emit(OP_MUL_RC, context);
            emit(r, context);
            emit(target, context);
            emit(cache_constant(NUM_AS_TVALUE(-1), context), context);
        }

        case TT_NOT: {
            if (node->cc) return cache_constant(NUM_AS_TVALUE(!TRUTHY(get_constant(target, context))), context);

            int r = free_register(register_first(context), context);
            emit(OP_NOT, context);
            emit(r, context);
            emit(target, context);
            emit(0x000, context);
        }
    }

    return r;
}

int compile_call(Node* node, Scope* context) {
    int target = compile(node->node.CallNode.target, context);
    int target_cc = node->node.CallNode.target->cc;
    int pre = target;

    int free_target = target_cc ? 1 : register_try_free(target, context) + 1;
    if (target_cc) target = register_constant(target, context, register_top(context));
    else {
        if (free_target) free_register(target, context);
        target = register_specific_top(target, context);

        if (pre != target) free_target = true;
    }

    for (int i = 0; i < node->node.CallNode.args.len; i++) assign_to_reg(node->node.CallNode.args.nodes[i], register_top(context), context);
    free_multiple(target + 1, node->node.CallNode.args.len, context);

    emit(OP_CALL, context);
    emit(free_target ? free_register(target, context) : target, context);
    emit(node->node.CallNode.args.len, context);
    return free_register(emit(register_first(context), context), context);
}

int compile_var_access(Node* node, Scope* context) {
    TValue tidx = get_symbol_register(context, node->node.VarAcess.name);

    if (IS_NIL(tidx)) {
        TValue global_idx = get_value_from_hashtable(globals, node->node.VarAcess.name);

        if (IS_NIL(global_idx)) {
            TValue details = make_fstring("identifier '%s' does not exist");
            details = format_string((TValue[2]){ details, node->node.VarAcess.name }, 2);

            call_error(new_unknown_identifier_error, AS_STRING(details)->str);
        }

        return free_register(register_global(global_idx.num, context, -1), context);
    }

    return (int)(AS_NUM(tidx));
}

int compile_number(Node* node, Scope* context) {
    return cache_constant(node->node.NumberNode.num, context);
}

int compile_string(Node* node, Scope* context) {
    return cache_constant(node->node.StringNode.str, context);
}

int compile_table(Node* node, Scope* context) {
    int r0 = free_register(register_top(context), context);
    int len = node->node.TableNode.values.len;

    for (int i = 0; i < len; i++) assign_to_reg(node->node.TableNode.values.nodes[i], register_top(context), context);
    free_multiple(r0, len, context);

    emit(OP_NEWTABLE, context);
    emit(r0, context);
    emit(len, context);
    return free_register(emit(register_first(context), context), context);
}

int compile_allocation(Node* node, Scope* context) {
    int target = compile(node->node.AllocNode.target, context);
    int target_cc = node->node.AllocNode.target->cc;
    int pre = target;

    int free_target = target_cc ? 1 : register_try_free(target, context) + 1;
    if (target_cc) target = register_constant(target, context, register_top(context));
    else {
        if (free_target) free_register(target, context);
        target = register_specific_top(target, context);

        if (pre != target) free_target = true;
    }

    for (int i = 0; i < node->node.AllocNode.args.len; i++) assign_to_reg(node->node.AllocNode.args.nodes[i], register_top(context), context);
    free_multiple(target + 1, node->node.AllocNode.args.len, context);

    emit(OP_ADDTABLE, context);
    emit(free_target ? free_register(target, context) : target, context);
    emit(node->node.CallNode.args.len, context);
    emit(0x00, context);

    return target;
}

int compile_if_statement(Node* node, Scope* context) {
    NodeArray cases = node->node.IfNode.cases;
    NodeArray stments = node->node.IfNode.statements;
    size_t next = 0;

    struct { size_t len; size_t ptr[cases.len]; } ends;
    ends.len = 0;

    for (int i = 0; i < cases.len; i++) {
        int condition = compile(cases.nodes[i], context);

        if (cases.nodes[i]->cc) {
            if (AS_NUM(get_constant(condition, context))) {
                compile_in_scope(stments.nodes[i], context);
                break;

            } else continue;
        }

        emit(OP_JFALSE, context);
        emit(condition, context);
        emit(0x00, context);
        emit(0x00, context);
        next = CURRENT_INSTR;

        compile_in_scope(stments.nodes[i], context);

        emit(OP_JUMP, context);
        emit(0x00, context);
        emit(0x00, context);
        emit(0x00, context);
        ends.ptr[ends.len++] = CURRENT_INSTR;

        patch_single_jfalse_label(context->bytecode.bytes, next, get_jump_lenght(next, context));
    }


    if (!node->cc) {
        if (node->node.IfNode.cases.len < node->node.IfNode.statements.len) {
            compile(stments.nodes[stments.len - 1], context);
            
        } else { context->bytecode.len -= 4; patch_single_jfalse_label(context->bytecode.bytes, next, get_jump_lenght(next, context)); }

        patch_array_jump_label(context->bytecode.bytes, (struct ArrayLabel){ .len = ends.len, .ptr = ends.ptr }, CURRENT_INSTR);
    }

    return 0;
}

int compile_nfor_loop(Node* node, Scope* context) {
    open_scope(context);

    Node* start_value_node = node->node.nForNode.values[0];
    Node* end_value_node = node->node.nForNode.values[1];
    Node* step_value_node = node->node.nForNode.values[2];

    int start_value = compile(node->node.nForNode.values[0], context);
    int r0 = start_value_node->cc ? register_constant(start_value, context, register_top(context)) : register_try_newtop(start_value, context);
    int r1 = register_top(context);
    int r2 = register_top(context);

    //assign_to_reg(step_value_node, r2, context);
    assign_to_reg(end_value_node, r1, context);
    assign_to_reg(step_value_node, r2, context);

    append_symbol(context, node->node.nForNode.identifier, r0);

    emit(OP_NFOR_INIT, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);
    size_t init = CURRENT_INSTR;

    compile(node->node.nForNode.statements, context);
    patch_single_jfalse_label(context->bytecode.bytes, init, get_jump_lenght(init, context));

    emit(OP_NFOR_LOOP, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);

    size_t loop = CURRENT_INSTR;
    patch_single_jfalse_label(context->bytecode.bytes, loop, get_jump_lenght_to(loop, init));

    free_multiple(r1, 2, context);
    close_scope(context);
}

int compile_gfor_loop(Node* node, Scope* context) {
    open_scope(context);

    int target = compile(node->node.gForNode.target, context);

    int r0 = register_top(context);
    int r1 = register_top(context);
    int r2 = register_register(target, context, register_top(context));

    append_symbol(context, node->node.gForNode.identifiers[0], r0);
    if (!IS_NIL(node->node.gForNode.identifiers[1])) append_symbol(context, node->node.gForNode.identifiers[1], r1);

    emit(OP_GFOR_INIT, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);
    size_t init = CURRENT_INSTR;

    compile(node->node.gForNode.statements, context);
    patch_single_jfalse_label(context->bytecode.bytes, init, get_jump_lenght(init, context));

    emit(OP_GFOR_LOOP, context);
    emit(r0, context);
    emit(0x00, context);
    emit(0x00, context);

    size_t loop = CURRENT_INSTR;
    patch_single_jfalse_label(context->bytecode.bytes, loop, get_jump_lenght_to(loop, init));

    close_scope(context);
}

int compile_while_loop(Node* node, Scope* context) {
    size_t pre_condition = CURRENT_INSTR;
    int condition = compile(node->node.whileNode.condition, context);
    
    emit(OP_JFALSE, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);
    size_t condition_label = CURRENT_INSTR;

    compile_in_scope(node->node.whileNode.statements, context);

    emit(OP_JUMP, context);
    emit(condition, context);
    emit(0x00, context);
    emit(0x00, context);

    patch_single_jump_label(context->bytecode.bytes, CURRENT_INSTR, get_jump_lenght_to(CURRENT_INSTR, pre_condition));
    patch_single_jfalse_label(context->bytecode.bytes, condition_label, get_jump_lenght(condition_label, context));
}

int compile_repeat_loop(Node* node, Scope* context) {
    size_t pre_statement = CURRENT_INSTR;
    compile_in_scope(node->node.whileNode.statements, context);
    int condition = compile(node->node.whileNode.condition, context);
    
    emit(OP_JFALSE, context);
    emit(condition, context);
    emit(0x00, context);
    emit(0x00, context);
    size_t condition_label = CURRENT_INSTR;

    emit(OP_JUMP, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    patch_single_jump_label(context->bytecode.bytes, CURRENT_INSTR, get_jump_lenght_to(CURRENT_INSTR, pre_statement));
    patch_single_jfalse_label(context->bytecode.bytes, condition_label, get_jump_lenght(condition_label, context));
}

int compile_require(Node* node, Scope* context) {
    TValue type = node->node.importNode.module;
    type = concat_string((TValue[2]){ make_fstring("Module "), type }, 2);

    TValue cached = get_value_from_hashtable(loaded_modules, type);

    if (!IS_NIL(cached)) compile_get_module(node, context, make_fstring("loadedReturns"), loaded_returns, type);
    else return compile_load_module(node, context, OP_REQUIRE, type);
}

int compile_import(Node* node, Scope* context) {
    TValue cached = get_value_from_hashtable(loaded_modules, node->type);

    if (!IS_NIL(cached)) compile_get_module(node, context, make_fstring("loaded"), loaded_modules, node->type);
    else return compile_load_module(node, context, OP_IMPORT, node->type);
}

int compile_from_import(Node* node, Scope* context) {
    TValue type = node->node.importNode.module;
    type = concat_string((TValue[2]){ make_fstring("Module "), type }, 2);
    
    Node import_node = { .type = type, .nt = NT_IMPORT };
    import_node.node.importNode.ast = node->node.fromImportNode.ast;
    import_node.node.importNode.module = node->node.fromImportNode.module;
    import_node.node.importNode.type_specifier = NIL;
    import_node.node.importNode.identifier = NIL;

    int module = compile(&import_node, context);
    register_specific(module, context);

    int argc = get_linked_list_len(node->node.fromImportNode.elements);
    TValue elements[argc];      linked_list_to_array(node->node.fromImportNode.elements, elements, argc);
    TValue nicknames[argc];     linked_list_to_array(node->node.fromImportNode.nicknames, nicknames, argc);

    for (int i = 0; i < argc; i++) {
        TValue class_namespace = get_value_from_hashtable(globals_map, type);
        TValue class_map = get_namespace_map(class_namespace);
        TValue idx = get_value_from_hashtable(class_map, elements[i]);

        int r = register_first(context);
        if (IS_NIL(nicknames[i])) append_symbol(context, elements[i], r);
        else append_symbol(context, nicknames[i], r);

        emit(OP_LOAD_FIELD, context);
        emit(module, context);
        emit(idx.num, context);
        emit(r, context);
    }

    return free_register(module, context);
}

int compile_func_def(Node* node, Scope* context) {
    Scope* func_context = new_scope();
    func_context->parent = context;

    int argc = get_linked_list_len(node->node.funcDefNode.args);
    TValue args[argc]; linked_list_to_array(node->node.funcDefNode.args, args, argc);

    for (int i = 0; i < argc; i++) {
        int r = register_first(func_context);
        append_symbol(func_context, args[i], r);
    }

    compile(node->node.funcDefNode.statements, func_context);
    TValue func = close_context(func_context);

    int idx = cache_constant(func, context);
    if (!IS_NIL(node->node.funcDefNode.name)) {
        idx = register_constant(idx, context, -1);
        append_symbol(context, node->node.funcDefNode.name, idx);
    }

    return idx;
}

int compile_method_def(Node* node, Scope* context) {
    TValue class_name = node->node.methodDefNode.class;
    int class_idx = get_global(class_name).num;

    Scope* func_context = new_scope();
    func_context->parent = context;

    int self_r = register_first(func_context);
    append_symbol(func_context, make_fstring("self"), self_r);

    if (node->nt == NT_CONSTRUCTOR) {
        int class_r = register_global(class_idx, context, -1);
        int field_count = get_namespace_value(get_value_from_hashtable(globals_map, undunder(class_name))).num;

        emit(OP_NEWOBJECT, func_context);
        emit(class_r, func_context);
        emit(field_count, func_context);
        emit(self_r, func_context);
    }

    int argc = get_linked_list_len(node->node.funcDefNode.args);
    TValue args[argc]; linked_list_to_array(node->node.funcDefNode.args, args, argc);

    for (int i = 0; i < argc; i++) {
        int r = register_first(func_context);
        append_symbol(func_context, args[i], r);
    }

    compile(node->node.funcDefNode.statements, func_context);
    TValue func = close_context(func_context);

    TValue class_namespace = get_value_from_hashtable(globals_map, class_name);
    TValue class_map = get_namespace_map(class_namespace);
    TValue tclass = get_namespace_value(class_namespace);

    TClass* class = AS_CLASS(tclass);
    int func_idx = get_value_from_hashtable(class_map, node->node.methodDefNode.name).num;
    class->fields[class->inherited_count + func_idx] = func;

    return 0;
}

int compile_class_def(Node* node, Scope* context) {
    int inherited_count = get_linked_list_len(node->node.classDefNode.inherited);
    TValue inherited_names[inherited_count]; linked_list_to_array(node->node.classDefNode.inherited, inherited_names, inherited_count);

    TValue inherited[inherited_count];
    for (int i = 0; i < inherited_count; i++) inherited[i] = get_value_from_hashtable(globals_map, inherited_names[i]);

    TValue class_name = dunder(node->node.classDefNode.name);

    TValue class_namespace = get_value_from_hashtable(globals_map, class_name);
    NamespaceEntry* class_entry = as_namespace_entry(class_namespace);
    TValue class_map = get_namespace_map(class_namespace);

    int field_count = class_entry->value.num;
    TValue class = new_class(inherited, NULL, inherited_count, field_count);

    class_entry->value = class;
    add_global(class, node->node.classDefNode.name);
    compile(node->node.classDefNode.statements, context);

    return 0;
}

int compile_selfcall(Node* node, Scope* context) {
    Node* target_node = node->node.SelfCallnode.target;
    int call_method = !is_dunder(target_node->type).num;
    int func;
    
    if (call_method) {
        Node var_access = { .cc = false, .nt = NT_VAR_ACESS, .node.VarAcess.name = target_node->type, .type = dunder(target_node->type) };
        Node field_acess = { .cc = false, .nt = NT_FIELDACCESS, .node.FieldAccessNode.name = node->node.SelfCallnode.name };
        field_acess.node.FieldAccessNode.target = &var_access;

        func = register_specific_top(compile(&field_acess, context), context);
        register_specific_top(compile(target_node, context), context);

    } else func = register_specific_top(get_target_field(target_node, node->node.SelfCallnode.name, !call_method, context), context);
    
    for (int i = 0; i < node->node.SelfCallnode.args.len; i++) assign_to_reg(node->node.SelfCallnode.args.nodes[i], register_top(context), context);
    free_multiple(func, node->node.SelfCallnode.args.len + 1 + call_method, context);

    emit(OP_CALL, context);
    emit(func, context);
    emit(node->node.SelfCallnode.args.len + call_method, context);
    return free_register(emit(register_first(context), context), context);
}

int compile_selfdeclare(Node* node, Scope* context) {
    TValue class_name = node->node.SelfDeclarenode.class;

    int value = node->node.SelfDeclarenode.value ? compile(node->node.SelfDeclarenode.value, context) : free_register(register_first(context), context);
    int free_value = 1;

    TValue class_namespace = get_value_from_hashtable(globals_map, class_name);
    NamespaceEntry* class_entry = as_namespace_entry(class_namespace);
    TValue class_map = get_namespace_map(class_namespace);

    if (TYPE(class_entry->value) != CLASS) {
        TValue self = get_symbol_register(context, make_fstring("self"));
        int r = self.num;

        if (node->node.VarDeclareNode.value->cc) value = register_constant(value, context, -1);
        else free_value = register_try_free(value, context) + 1;

        emit(OP_SETFIELD, context);
        emit(r, context);
        emit(get_value_from_hashtable(class_map, node->node.SelfDeclarenode.var_name).num, context);
        emit(free_value ? free_register(value, context) : value, context);

    } else {
        TValue idx = get_value_from_hashtable(class_map, node->node.SelfDeclarenode.var_name);
        TValue tclass = class_entry->value;
        TClass* class = AS_CLASS(tclass);

        if (node->node.SelfDeclarenode.value && node->node.SelfDeclarenode.value->cc) {
            class->fields[class->inherited_count + (int)idx.num] = get_constant(value, context);
            return value;

        }  else free_value = register_try_free(value, context) + 1;

        int class_r = free_register(register_global(get_value_from_hashtable(globals, undunder(class_name)).num, context, -1), context);

        emit(OP_SETFIELD, context);
        emit(class_r, context);
        emit(idx.num, context);
        emit(free_value ? free_register(value, context) : value, context);
    }
}

int compile_fieldaccess(Node* node, Scope* context) {
    Node* target_node = node->node.FieldAccessNode.target;
    return get_target_field(target_node, node->node.FieldAccessNode.name, 1, context);
}

int compile_constructor(Node* node, Scope* context) {
    return compile_method_def(node, context);
}

int compile_indexing(Node* node, Scope* context) {
    Node* target_node = node->node.IndexNode.target;
    Node* index_node = node->node.IndexNode.index;

    int target = compile(target_node, context);
    int free_target = target_node->cc ? 1 : register_try_free(target, context) + 1;
    int index = compile(node->node.IndexNode.index, context);

    if (target_node->cc) target = register_constant(target, context, -1);
    if (index_node->cc) index = free_register(register_constant(index, context, -1), context);
    if (free_target) free_register(target, context);

    emit(OP_GETTABLE, context);
    emit(target, context);
    emit(index, context);
    return emit(free_register(register_first(context), context), context);
}


Scope* new_scope() {
    Scope* scope = malloc(sizeof(Scope));
    scope->parent = NULL;
    scope->symbol_stack = AS_TABLE(new_table(NULL, 0));
    open_scope(scope);

    size_t CONST_POOL_SIZE = sizeof(scope->const_pool.const_pool) / TVALUESIZE;
    for (int i = 0; i < CONST_POOL_SIZE; i++) { scope->const_pool.const_pool[i] = NIL; }
    scope->const_pool.len = 0;

    scope->free_regs.len = 0;
    scope->reg_ptr = 0;
    scope->breaks.size = 0;
    scope->breaks.next = NULL;
    scope->continues.size = 0;
    scope->continues.next = NULL;

    scope->bytecode.bytes = malloc(64);
    scope->bytecode.len = 0;
    scope->bytecode.cap = 64;
    scope->bytecode.bytes[0] = OP_HALT;

    return scope;
}

int emit(uint8_t instr, Scope* context) {
    context->bytecode.bytes[context->bytecode.len++] = instr;
    if (context->bytecode.len == context->bytecode.cap) {
        uint8_t* new = realloc(context->bytecode.bytes, (context->bytecode.cap *= 2));
        if (new == NULL) call_error(new_failed_allocation_error, "Failed allocation while emiting instruction");

        context->bytecode.bytes = new;
    }

    return instr;
}

static int register_first(Scope* context) {
    if (context->free_regs.len > 0) return context->free_regs.free_regs[context->free_regs.len - 1];
    return register_top(context);
}

static int free_register(uint8_t idx, Scope* context) {
    if (idx != context->reg_ptr - 1) {
        for (int i = 0; i < context->free_regs.len; i++) {
            if (context->free_regs.free_regs[i] == idx) return idx;
        }

        return (context->free_regs.free_regs[context->free_regs.len++] = idx);
    }

    return --context->reg_ptr;
}

static int register_constant(uint8_t const_i, Scope* context, int16_t r_) {
    uint8_t r = r_ + 1 ? r_ : register_first(context);
    emit(OP_LOAD_CR, context);
    emit(const_i, context);
    emit(register_specific(r, context), context);
    emit(0x00, context);

    return r;
}

static int register_register(uint8_t r, Scope* context, int16_t r_) {
    uint8_t r2 = r_ + 1 ? r_ : register_first(context);
    if (r2 != r) {
        emit(OP_LOAD_RR, context);
        emit(r, context);
        emit(register_specific(r2, context), context);
        emit(0x00, context);

    } else register_specific(r2, context);

    return r2;
}

static int register_global(uint16_t glob_i, Scope* context, int16_t r_) {
    uint8_t r = r_ + 1 ? r_ : register_first(context);
    emit(OP_LOAD_GLOBAL, context);
    emit(INT32_TO_HI(glob_i), context);
    emit(INT32_TO_LO(glob_i), context);
    emit(register_specific(r, context), context);

    return r;
}

static int register_try_free(uint8_t r, Scope* context) {
    for (int i = 0; i < context->free_regs.len; i++) {
        if (r == context->free_regs.free_regs[i]) {
            for (int i2 = i; i2 < context->free_regs.len - 1; i2++) {
                context->free_regs.free_regs[i2] = context->free_regs.free_regs[i2 + 1];
            }

            context->free_regs.len--;
            return r;
        }
    }

    if (r == context->reg_ptr) return context->reg_ptr++;
    return -1;
}

static int register_try_newtop(uint8_t r, Scope* context) {
    if (r == context->reg_ptr) return context->reg_ptr++;
    else return register_register(r, context, register_top(context));
}

static int register_specific_top(uint8_t r, Scope* context) {
    if (r == context->reg_ptr - 1) return r;
    else if (r == context->reg_ptr) return context->reg_ptr++;
    else return register_register(r, context, register_top(context));
}

static int free_multiple(uint8_t r0, uint8_t len, Scope* context) {
    for (int i = len - 1; i >= 0; i--) {
        free_register(r0 + i, context);
    }

    return r0;
}

static int get_constant_idx(TValue const_v, Scope* context) {
    for (int i = 0; i < context->const_pool.len; i++) {
        if (AS_TVALUE(context->const_pool.const_pool[i]) == AS_TVALUE(const_v)) return i;
    }

    return -1;
}

static int cache_constant(TValue const_v, Scope* context) {
    int idx = get_constant_idx(const_v, context);
    if (idx != -1) return idx;

    if (context->const_pool.len == 0xff) call_error(new_stack_overflow_error, "Fatal Error : Constant overflow, exceeded constants limit");
    context->const_pool.const_pool[context->const_pool.len++] = const_v;

    return context->const_pool.len - 1;
}

static inline int register_top(Scope* context) {
    if (context->reg_ptr == 0xff) call_error(new_stack_overflow_error, "Fatal Error : Register overflow, exceeded register limit");
    return context->reg_ptr++;
}

static inline int register_specific(uint8_t r, Scope* context) {
    register_try_free(r, context);
    return r;
}

static inline int register_try(uint8_t r, Scope* context) {
    int new_r = register_try_free(r, context);
    if (!(new_r + 1)) register_first(context); else return r;
}

static inline int correct_constant_idx(int idx, Scope* context) { return idx < context->const_pool.len ? 1 : 0; }

static inline void delete_constant(int idx, Scope* context) {
    shift_tvalue_array(context->const_pool.const_pool, context->const_pool.len, idx);
    context->const_pool.len--;
}

static inline TValue get_constant(int idx, Scope* context) { return context->const_pool.const_pool[idx]; }

static inline int assignement_analysis(Node* node) {
    int depth = 0;
    int target_count = 0;
    return full_assignement_analysis(node->node.AssignementNode.value, node->node.AssignementNode.lvalue, &depth, &target_count);
}

static inline int assign_to_reg(Node* node, uint8_t r, Scope* context) {
    int depth = 0; int target_count = 0;
    int analysis = full_assignement_analysis(node, (Node[1]){ (Node){ .nt = NT_PLACEHOLDER } }, &depth, &target_count);
    if (analysis) free_register(r, context);

    int end_value = compile(node, context);
    if (node->cc) register_constant(end_value, context, r); else register_register(end_value, context, r);

    return r;
}

static int full_assignement_analysis(Node* node, Node* target, int* depth, int* target_count) {
    #define ADD_DEPTH ( ((*depth)++), depth )

    if (node_equal(target, node)) target_count++;
    if (*depth > 1 && *target_count > 0) return 0;

    int endv;
    switch (node->nt) {
        case NT_VAR_DECLARE: return 0;
        case NT_UNI_OP: return full_assignement_analysis(node->node.UniOpNode.target, target, ADD_DEPTH, target_count);

        case NT_ASSIGNEMENT: return 
            full_assignement_analysis(node->node.AssignementNode.lvalue, target, ADD_DEPTH, target_count) &&
            full_assignement_analysis(node->node.AssignementNode.value, target, depth, target_count);

        case NT_BIN_OP: return full_assignement_analysis(node->node.BinOpNode.left, target, depth, target_count) &&
                               full_assignement_analysis(node->node.BinOpNode.right, target, ADD_DEPTH, target_count);

        case NT_FIELDACCESS: return
            full_assignement_analysis(node->node.FieldAccessNode.target, target, ADD_DEPTH, target_count);

        case NT_TABLE:
            endv = 1;
            
            for (int i = 0; i < node->node.TableNode.values.len; i++)
                endv = endv && full_assignement_analysis(node->node.TableNode.values.nodes[i], target, ADD_DEPTH, target_count);

            return endv;
        
        case NT_SELFCALL: *depth += 1;
        case NT_CALL:
            endv = 1;

            for (int i = 0; i < node->node.CallNode.args.len; i++)
                endv = endv && full_assignement_analysis(node->node.CallNode.args.nodes[i], target, ADD_DEPTH, target_count);

            return endv;

        default: return 1;
    }
}


static void open_scope(Scope* context) {
    append_to_table(TBL_TO_TVALUE(context->symbol_stack), new_table(NULL, 0));
    update_symbols(context);
}

static void close_scope(Scope* context) {
    TValue last_scope = get_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1));
    TValue registers = hashed_values_table((TValue[1]){ last_scope }, 1);
    counting_sort(registers);

    TTable* registers_tbl = AS_TABLE(registers);

    for (int i = 0; i < registers_tbl->array.len; i++) { free_register((uint8_t)AS_NUM(registers_tbl->array.array[i]), context); }
    
    free_table(&last_scope, 1);
    free_table(&registers, 1);
    remove_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1));
    update_symbols(context);
}

static inline int compile_in_scope(Node* node, Scope* context) {
    open_scope(context);
    int res = compile(node, context);
    close_scope(context);

    return res;
}

static inline void update_symbols(Scope* context) {
    if (!context->symbol_stack->array.len) return;
    context->symbols = AS_TABLE(get_from_table(TBL_TO_TVALUE(context->symbol_stack), NUM_AS_TVALUE(context->symbol_stack->array.len - 1)));
}

static TValue append_symbol(Scope* context, TValue name, int r) { add_value_to_hashtable(TBL_TO_TVALUE(context->symbols), name, NUM_AS_TVALUE(r)); }

static TValue get_symbol_register(Scope* context, TValue name) {
    for (int i = context->symbol_stack->array.len - 1; i >= 0; i--) {
        TValue symbol_table = context->symbol_stack->array.array[i];
        TValue tidx = get_value_from_hashtable(symbol_table, name);
        if (!IS_NIL(tidx)) return tidx;
    }

    return NIL;
}

static int get_target_field(Node* target_node, TValue name, int free_target, Scope* context) {
    int target = compile(target_node, context);

    if (target_node->cc) target = register_constant(target, context, -1);
    else free_target = register_try_free(target, context) + 1 && free_target;

    TValue class_name = target_node->type;

    TValue class_namespace = get_value_from_hashtable(globals_map, class_name);
    TValue class_map = get_namespace_map(class_namespace);
    TValue idx = get_value_from_hashtable(class_map, name);

    emit(OP_LOAD_FIELD, context);
    emit(free_target ? free_register(target, context) : target, context);
    emit(idx.num, context);
    return emit(free_register(register_first(context), context), context);
}


int compile_boolean_op(int left, int right, int leftCC, int rightCC, TokenType op, Scope* context) {

}

int compile_load_module(Node* node, Scope* context, BytecodeInstruction instr, TValue type) {
    Scope* import_context = new_scope();
    compile(node->node.importNode.ast, import_context);

    TValue map = TBL_TO_TVALUE(import_context->symbols);
    map = copy_table(&map, 1);

    TValue namespace = table_as_namespace(map);

    TValue module = close_context(import_context);
    int mod_idx = module_load(module, type);
    module_load_returns(NUM_AS_TVALUE(0), type);

    int r = register_first(context);
    if (!IS_NIL(node->node.importNode.identifier)) append_symbol(context, node->node.importNode.identifier, r);
    else free_register(r, context);

    emit(instr, context);
    emit(r, context);
    emit(INT32_TO_HI(mod_idx), context);
    emit(INT32_TO_LO(mod_idx), context);

    add_value_to_hashtable(globals_map, type, namespace);
    return r;
}

int compile_get_module(Node* node, Scope* context, TValue field, TValue cache, TValue type) {
    Node var_access = { .nt = NT_VAR_ACESS, .node.VarAcess.name = make_fstring("Module"), .type = make_fstring("__Module") };
    Node field_access = { .nt = NT_FIELDACCESS, .node.FieldAccessNode.name = field, .node.FieldAccessNode.target = &var_access };

    int loaded = compile(&field_access, context); register_specific(loaded, context);
    int loaded_idx = cache_constant(get_value_from_hashtable(cache, type), context);

    loaded_idx = free_register(register_constant(loaded_idx, context, -1), context);
    free_register(loaded, context);

    int r = register_first(context);
    if (!IS_NIL(node->node.importNode.identifier)) append_symbol(context, node->node.importNode.identifier, r);
    else free_register(r, context);

    emit(OP_GETTABLE, context);
    emit(loaded, context);
    emit(loaded_idx, context);
    return emit(r, context);
}


TValue close_context(Scope* context) {
    emit(OP_HALT, context);
    emit(0x00, context);
    emit(0x00, context);
    emit(0x00, context);

    uint8_t* bytecode = realloc(context->bytecode.bytes, context->bytecode.len);
    if (!bytecode) call_error(new_failed_allocation_error, "Failed allocation while closing compile context");

    TValue func = new_func(bytecode, context->const_pool.const_pool, context->const_pool.len, NULL);

    close_scope(context);
    free_table((TValue[1]){ TBL_TO_TVALUE(context->symbol_stack) }, 1);
    return func;
}
