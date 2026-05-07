#include <stdlib.h>

#include "Globals/string.h"
#include "Parser/parser.h"


static Node* bin_op(ParseResult* res, ParseFunc func1, Token ops[], int opc, ParseFunc func2) {
    if (func2 == NULL) func2 = func1;

    Node* left = func1(res);
    while (is_correct_operator(*res->tokens, ops, opc)) {
        Token op = *res->tokens; res->tokens++;
        Node* right = func2(res);

        Node* n_left = new_node(NT_BIN_OP);
        n_left->node.BinOpNode.left = left;
        n_left->node.BinOpNode.op = op;
        n_left->node.BinOpNode.right =  right;
        left = n_left;
    }

    return left;
}

static NodeArray separated(ParseResult* res, ParseFunc func, Token separator) {
    NodeArray nda = make_node_array();

    do {
        Node* elem = func(res);
        if (elem == NULL) break;

        append_to_node_array(&nda, elem);

        if (!match_token(*res->tokens, SEPARATOR)) break;
        res->tokens++;
        clean_newlines(&res->tokens, &res->adv_count);

    } while (1);

    res->err = NULL;
    return nda;
}


Node* parse(Token* tokens, char filename[]) {
    init_operators();
    ParseResult res = new_res(tokens);

    Node* ast = statements(&res);
    if (res.err != NULL) call_error(res.err, filename);
    return ast;
}

Node* numerics(ParseResult* res) {
    Node* return_node = new_node(NT_PLACEHOLDER);

    switch (res->tokens->tt) {
        case TT_IDENTIFIER : {
            return_node->nt = NT_VAR_ACESS;
            return_node->node.VarAcess.name = res->tokens->value;
            break;
        }

        case TT_NUMBER : {
            return_node->nt = NT_NUMBER;
            return_node->node.NumberNode.num = res->tokens->value;
            break;
        }

        case TT_STR : {
            return_node->nt = NT_STRING;
            return_node->node.StringNode.str = res->tokens->value;
            break;
        }

        case TT_LPAR : {
            res->tokens++;
            Node* nexpr = expr(res);
            except(res, TT_RPAR, "(RPAR : ')' ) EXCEPTED", NULL, NULL);

            return_node->nt = nexpr->nt;
            return_node->node = nexpr->node;
            free(nexpr); res->tokens--;
            break;
        }

        default : {
            res->err = new_error("InvalidSyntaxError", "(INT | STR | OPERATOR) Excepted", 0, 0);
            free(return_node);
            return NULL;
        }
    }

    res->tokens++;
    return return_node;
}

Node* call(ParseResult* res) {
    Node* num = numerics(res);

    if (match_tt(*res->tokens, TT_LPAR)) {
        clean_newlines(&res->tokens, &res->adv_count); res->tokens++;
        NodeArray args =  separated(res, expr, SEPARATOR);
        
        except(res, TT_RPAR, "(RPAR : ')') EXCEPTED", NULL, NULL);
        Node* return_node = new_node(NT_CALL);
        return_node->node.CallNode.args = args;
        return_node->node.CallNode.target = num;

        return return_node;
    }

    return num;
}

Node* power(ParseResult* res) { return bin_op(res, call, POWER_OPERATORS, POWER_OPERATORS_SIZE, factor); }

Node* factor(ParseResult* res) {
    if (is_correct_operator(*res->tokens, FACTOR_OPERATORS, FACTOR_OPERATORS_SIZE)) {
        Token op = *res->tokens++;
        Node* target = factor(res);

        Node* return_node = new_node(NT_UNI_OP);
        return_node->node.UniOpNode.op = op;
        return_node->node.UniOpNode.target = target;

        return return_node;
    }

    return power(res);
}

Node* term(ParseResult* res) { return bin_op(res, factor, TERM_OPERATORS, TERM_OPERATORS_SIZE, NULL); }

Node* arithmetic(ParseResult* res) { return bin_op(res, term, ARITHMETIC_OPERATORS, ARITHMETIC_OPERATORS_SIZE, NULL); }

Node* comparaison(ParseResult* res) {
    if (match_tt(*res->tokens, TT_NOT)) {
        Token op = *res->tokens;
        Node* target = comparaison(res);

        Node* return_node = new_node(NT_UNI_OP);
        return_node->node.UniOpNode.op = op;
        return_node->node.UniOpNode.target = target;

        return return_node;
    }

    return bin_op(res, arithmetic, COMP_OPERATORS, COMP_OPERATORS_SIZE, NULL);
}

Node* logical(ParseResult* res) { return bin_op(res, comparaison, LOGICAL_OPERATORS, LOGICAL_OPERATORS_SIZE, NULL); }

Node* expr(ParseResult* res) {
    if (match_keyword(*res->tokens, "var")) {
        res->tokens++;

        TValue var_name;
        except(res, TT_IDENTIFIER, "(IDENTIFIER) EXCEPTED", &var_name, NULL);
        except(res, TT_EQ, "(EQ : '=') EXCEPTED", NULL, NULL);

        Node* nexpr = expr(res);
        Node* return_node = new_node(NT_VAR_DECLARE);
        return_node->node.VarDeclareNode.var_name = var_name;
        return_node->node.VarDeclareNode.value = nexpr;

        return return_node;

    }

    Node* log =  logical(res);
    if (res->tokens->tt == TT_EQ) {
        res->tokens++;
        Node* exp = expr(res);

        Node* new = new_node(NT_ASSIGNEMENT);
        new->node.AssignementNode.lvalue = log;
        new->node.AssignementNode.value = exp;
        log = new;
    }

    return log;
}

Node* statement(ParseResult* res) {
    if (match_keyword(*res->tokens, "return")) {
        res->tokens++; NodeArray values =  separated(res, expr, SEPARATOR);
        Node* return_node = new_node(NT_RETURN);
        return_node->node.ReturnNode = values;

        return return_node;

    } else if (match_keyword(*res->tokens, "break")) { res->tokens++; return new_node(NT_BREAK); }
    else if (match_keyword(*res->tokens, "continue")) { res->tokens++; return new_node(NT_CONTINUE); }

    return expr(res);
}

Node* statements(ParseResult* res) {
    Node* node = new_node(NT_STATEMENTS);
    NodeArray statementNodes = make_node_array();

    while (res->tokens->tt != TT_EOF) {
        clean_newlines(&res->tokens, &res->adv_count);
        Node* stmnt = statement(res);

        if (stmnt == NULL || res->err != NULL) break;
        append_to_node_array(&statementNodes, stmnt);
    }

    node->node.StatementsNode = statementNodes;
    return node;
}


static inline void clean_newlines(Token** tokens, size_t* adv_count) { while ( (**tokens).tt == TT_NEWLINE && ++*adv_count ) ++*tokens; }
static inline ParseResult new_res(Token* tokens) { return (ParseResult){ .err = NULL, .adv_count = 0, .tokens = tokens }; }
static inline Node* new_node(NodeType nt) { Node* node =  malloc(sizeof(Node)); node->nt = nt; node->cc = false; return node; }


static inline void clean_node_array(NodeArray* nda, size_t start_index) {
    for (int i = start_index; i < nda->cap; i++) nda->nodes[i] = NULL;
}

static NodeArray make_node_array() {
    size_t cap = 8;
    Node** ndarray = malloc(sizeof(Node*) * cap);
    if (ndarray == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making node");

    NodeArray nda = { .len = 0, .cap = cap, .nodes = ndarray };
    clean_node_array(&nda, 0);
    return nda;
}

static Node* append_to_node_array(NodeArray* nda, Node* node) {
    if (nda->len >= nda->cap) {
        nda->nodes = realloc(nda->nodes, sizeof(Node*) * (nda->cap *= 2));
        if (nda->nodes == NULL) FAILED_ALLOCATION_ERROR("Failed allocation while making node");

        clean_node_array(nda, nda->cap / 2);
    }

    nda->nodes[nda->len++] = node;
}


static inline bool is_correct_operator(Token op, Token ops[], int opc) {
    for (int i = 0; i < opc; i++) {
        if (match_token(op, ops[i])) return true;
    }

    return false;
}

static inline void except(ParseResult* res, TokenType tt, char* excepted, TValue* value, char* kword) {
    if (res->tokens->tt != tt || (kword && !match_keyword(*res->tokens, kword))) SYNTAX_ERROR(excepted);
    if (value != NULL) *value = res->tokens->value;
    *res->tokens++;
}



