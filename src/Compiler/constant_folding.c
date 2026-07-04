#include "Compiler/constant_folding.h"
#include "Globals/string.h"
#include <math.h>
#include <stdio.h>


FoldingFunction fold_functions[] = {
    [NT_STATEMENTS]     = fold_statements,
    [NT_RETURN]         = fold_return,
    [NT_BREAK]          = fold_break,
    [NT_CONTINUE]       = fold_continue,
    [NT_VAR_DECLARE]    = fold_var_declare,
    [NT_ASSIGNEMENT]    = fold_assignement,
    [NT_BIN_OP]         = fold_bin_op,
    [NT_UNI_OP]         = fold_uni_op,
    [NT_CALL]           = fold_call,
    [NT_VAR_ACESS]      = fold_var_access,
    [NT_NUMBER]         = fold_number,
    [NT_STRING]         = fold_string,
    [NT_TABLE]          = fold_table,
    [NT_ALLOCATION]     = fold_allocation,
    [NT_IF]             = fold_if_statement,
    [NT_NFOR]           = fold_nfor_loop,
    [NT_GFOR]           = fold_gfor_loop,
    [NT_WHILE]          = fold_while_loop,
    [NT_REPEAT]         = fold_repeat_loop,
    [NT_IMPORT]         = fold_import,
    [NT_FUNCDEF]        = fold_func_def,
    [NT_METHODDEF]      = fold_func_def,
    [NT_CLASSDEF]       = fold_class_def,
    [NT_SELFCALL]       = fold_call,
    [NT_SELFDECLARE]    = fold_var_declare,
    [NT_FIELDACCESS]    = fold_fieldaccess,
    [NT_CONSTRUCTOR]    = fold_func_def,
    [NT_INDEXING]       = fold_indexing
};


TValue fold(Node* node) { return fold_functions[node->nt](node); }

TValue fold_statements(Node* node) {
    for (int i = 0; i < node->node.StatementsNode.len; i++) {
        Node* new = node->node.StatementsNode.nodes[i]; fold(new);

        if (new->cc) {
            free(new); i--; node->node.StatementsNode.len--;
            for (int ii = i + 1; ii < node->node.StatementsNode.len; ii++) node->node.StatementsNode.nodes[ii] = node->node.StatementsNode.nodes[ii + 1];
        }
    }

    return (node->cc = false, NIL);
}

TValue fold_return(Node* node) {
    for (int i = 0; i < node->node.ReturnNode.len; i++) {
        fold(node->node.ReturnNode.nodes[i]);
    }

    return (node->cc = false, NIL);
}

TValue fold_break(Node* node) { return (node->cc = false, NIL); }

TValue fold_continue(Node* node) { return (node->cc = false, NIL); }

TValue fold_var_declare(Node* node) { return (node->cc = false, node->node.VarDeclareNode.value ? fold(node->node.VarDeclareNode.value) : NIL); }

TValue fold_assignement(Node* node) { fold(node->node.AssignementNode.lvalue); return (node->cc = false, fold(node->node.AssignementNode.value)); }

TValue fold_bin_op(Node* node) {
    TValue left = fold(node->node.BinOpNode.left);
    TValue right = fold(node->node.BinOpNode.right);
    TValue res = NIL;

    if (!node->node.BinOpNode.left->cc || !node->node.BinOpNode.right->cc) return (node->cc = false, NIL);

    switch (node->node.BinOpNode.op.tt) {
        case TT_EEQ:    res = NUM_AS_TVALUE( EQUAL(left, right) ); break;
        case TT_NEQ:    res = NUM_AS_TVALUE( !EQUAL(left, right) ); break;
        case TT_PLUS:   res = NUM_AS_TVALUE( left.num + right.num ); break;
        case TT_MINUS:  res = NUM_AS_TVALUE( left.num - right.num ); break;
        case TT_MUL:    res = NUM_AS_TVALUE( left.num * right.num ); break;
        case TT_DIV:    res = NUM_AS_TVALUE( left.num / right.num ); break;
        case TT_IDIV:   res = NUM_AS_TVALUE( floor(left.num / right.num) ); break;
        case TT_MOD:    res = NUM_AS_TVALUE( (int64_t)left.num % (int64_t)right.num ); break;
        case TT_AND:    res = NUM_AS_TVALUE( TRUTHY(left) && TRUTHY(right) ); break;
        case TT_OR:     res = NUM_AS_TVALUE( TRUTHY(left) || TRUTHY(right) ); break;
        case TT_BAND:   res = NUM_AS_TVALUE( left.nan & right.nan ); break;
        case TT_BOR:    res = NUM_AS_TVALUE( left.nan | right.nan ); break;
        case TT_BXOR:   res = NUM_AS_TVALUE( left.nan ^ right.nan ); break;
        case TT_RAND:   res = TRUTHY(left) ? right : left; break;
        case TT_ROR:    res = TRUTHY(left) ? left : right; break;
        case TT_CONCAT: res = concat_string((TValue[2]){ left, right }, 2); break;
        case TT_GT:     res = NUM_AS_TVALUE( left.num > right.num ); break;
        case TT_GTEQ:   res = NUM_AS_TVALUE( left.num >= right.num ); break;
        case TT_LT:     res = NUM_AS_TVALUE( left.num < right.num ); break;
        case TT_LTEQ:   res = NUM_AS_TVALUE( left.num <= right.num ); break;
        case TT_LSHIFT: res = NUM_AS_TVALUE( left.nan << right.nan ); break;
        case TT_RSHIFT: res = NUM_AS_TVALUE( left.nan >> right.nan ); break;
        case TT_POW:    res = NUM_AS_TVALUE( pow(left.num, right.num) ); break;
    }

    free(node->node.BinOpNode.left);
    free(node->node.BinOpNode.right);

    switch (TYPE(res)) {
        case STRING: {
            node->nt = NT_STRING; node->cc = true;
            return node->node.StringNode.str = res;
        }

        default: {
            node->nt = NT_NUMBER; node->cc = true;
            return node->node.NumberNode.num = res;
        }
    }
}

TValue fold_uni_op(Node* node) {
    TValue target = fold(node->node.UniOpNode.target);
    TValue res = NIL;
    if (!node->node.UniOpNode.target->cc) return (node->cc = false, NIL);

    switch (node->node.UniOpNode.op.tt) {
        case TT_MINUS:  res = NUM_AS_TVALUE(-target.num); break;
        case TT_NOT:    res = NUM_AS_TVALUE( FALSY(target) ); break;
    }

    free(node->node.UniOpNode.target);
    node->nt = NT_NUMBER; node->cc = true;
    return node->node.NumberNode.num = res;
}

TValue fold_call(Node* node) {
    fold(node->node.CallNode.target);

    for (int i = 0; i < node->node.CallNode.args.len; i++) {
        fold(node->node.CallNode.args.nodes[i]);
    }

    return (node->cc = false, NIL);
}

TValue fold_var_access(Node* node) { return (node->cc = false, NIL); }

TValue fold_number(Node* node) { return (node->cc = true, node->node.NumberNode.num); }

TValue fold_string(Node* node) { return (node->cc = true, node->node.StringNode.str); }

TValue fold_table(Node* node) {
    for (int i = 0; i < node->node.TableNode.values.len; i++) {
        fold(node->node.TableNode.values.nodes[i]);
    }
    
    return (node->cc = false, NIL);
}

TValue fold_allocation(Node* node) {
    fold(node->node.AllocNode.target);
    for (int i = 0; i < node->node.AllocNode.args.len; i++) {
        fold(node->node.AllocNode.args.nodes[i]);
    }

    return (node->cc = false, NIL);
}

TValue fold_if_statement(Node* node) {
    for (int i = 0; i < node->node.IfNode.cases.len; i++) {
        TValue cond = fold(node->node.IfNode.cases.nodes[i]);
        fold(node->node.IfNode.statements.nodes[i]);

        if (node->node.IfNode.cases.nodes[i]->cc) {
            if (TRUTHY(cond)) { node->node.IfNode.cases.len = i; break; }
            else {
                node->node.IfNode.cases.len--; node->node.IfNode.statements.len--;
                for (int ii = i; ii < node->node.IfNode.cases.len; ii++) node->node.IfNode.cases.nodes[ii] = node->node.IfNode.cases.nodes[ii + 1];
                for (int ii = i; ii < node->node.IfNode.statements.len; ii++) node->node.IfNode.statements.nodes[ii] = node->node.IfNode.statements.nodes[ii + 1];
            }
        }
    }

    return (node->cc = false, NIL);
}

TValue fold_nfor_loop(Node* node) {
    fold(node->node.nForNode.values[0]);
    fold(node->node.nForNode.values[1]);
    fold(node->node.nForNode.values[2]);
    fold(node->node.nForNode.statements);

    return (node->cc = false, NIL);
}

TValue fold_gfor_loop(Node* node) {
    fold(node->node.gForNode.target);
    fold(node->node.gForNode.statements);

    return (node->cc = false, NIL);
}

TValue fold_while_loop(Node* node) {
    TValue cond = fold(node->node.whileNode.condition);
    if (FALSY(cond)) { free(node); return (node->cc = true, NIL); }

    fold(node->node.whileNode.statements);
    return (node->cc = false, NIL);
}

TValue fold_repeat_loop(Node* node) {
    fold(node->node.repeatNode.condition);
    fold(node->node.repeatNode.statements);

    return (node->cc = false, NIL);
}

TValue fold_import(Node* node) {

}

TValue fold_func_def(Node* node) {
    fold(node->node.funcDefNode.statements);
    return (node->cc = false, NIL);
}

TValue fold_class_def(Node* node) {
    fold(node->node.classDefNode.statements);
    return (node->cc = false, NIL);
}

TValue fold_fieldaccess(Node* node) {
    fold(node->node.FieldAccessNode.target);
    return (node->cc = false, NIL);
}

TValue fold_indexing(Node* node) {
    fold(node->node.IndexNode.target);
    fold(node->node.IndexNode.index);

    return (node->cc = false, NIL);
}
