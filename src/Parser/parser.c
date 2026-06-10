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

static Node* postfix_op(ParseResult* res, Node* num) {
    switch (res->tokens->tt) {
        case TT_LPAR: {
            clean_newlines(&res->tokens, &res->adv_count); res->tokens++;
            NodeArray args =  separated(res, expr, SEPARATOR);
            
            EXPECTED(expect(res, TT_RPAR, "(RPAR : ')') EXPECTED", NULL, NULL))
            Node* return_node = new_node(NT_CALL);
            return_node->node.CallNode.args = args;
            return_node->node.CallNode.target = num;

            return return_node;
        }

        case TT_LSBRAC: {

        }

        case TT_DOT: {
            
        }
    }

    return NULL;
}


Node* parse(Token* tokens, char filename[]) {
    init_operators();
    ParseResult res = new_res(tokens);

    Node* ast = statements(&res);
    if (res.err != NULL) call_error(res.err, filename);
    return ast;
}

Node* alloc_expr(ParseResult* res) {
    res->tokens++;

    Node* target = expr(res);
    EXPECTED(expect(res, TT_ARROW, "(ARROW) EXPECTED", NULL, NULL))

    clean_newlines(&res->tokens, &res->adv_count); res->tokens++;
    NodeArray args =  separated(res, expr, SEPARATOR);

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))
    
    Node* return_node = new_node(NT_ALLOCATION);
    return_node->node.AllocNode.args = args;
    return_node->node.AllocNode.target = target;

    return return_node;
}

Node* if_expr(ParseResult* res) {
    NodeArray cases = make_node_array();
    NodeArray stments = make_node_array();

    do {
        res->tokens++;

        Node* condition = expr(res);
        EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'then') EXPECTED", NULL, "then"))
        Node* thenStatements = get_statements(res, ELSEIF_TOKEN);
        
        append_to_node_array(&cases, condition);
        append_to_node_array(&stments, thenStatements);
    } while( match_keyword(*res->tokens, "elseif") );

    if (match_keyword(*res->tokens, "else")) {
        res->tokens++;
        Node* thenStatements = get_statements(res, END_TOKEN);
        append_to_node_array(&stments, thenStatements);
    }

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))

    Node* ifNode = new_node(NT_IF);
    ifNode->node.IfNode.cases = cases;
    ifNode->node.IfNode.statements = stments;

    return ifNode;
}

Node* for_expr(ParseResult* res) {
    res->tokens++;
    Node* return_node;
    
    TValue i;
    TValue v = NIL;
    EXPECTED(expect(res, TT_IDENTIFIER, "(IDENTIFIER) EXPECTED", &i, NULL))
    
    switch (res->tokens++->tt) {
        case TT_EQ: {
            Node* baseValue = expr(res);
            EXPECTED(expect(res, TT_SEPARATOR, "(SEPARATOR:',') EXPECTED", NULL, NULL))
            Node* endValue = expr(res);

            Node* stepValue;
            if (res->tokens->tt == TT_SEPARATOR) { res->tokens++; stepValue = expr(res); }
            else (stepValue = new_node(NT_NUMBER))->node.NumberNode.num = NUM_AS_TVALUE(1);

            EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'repeat') EXPECTED", NULL, "repeat"))
            Node* stments = get_statements(res, END_TOKEN);

            Node* nfor_node = new_node(NT_NFOR);
            nfor_node->node.nForNode.identifier = i;
            nfor_node->node.nForNode.values[0] = baseValue;
            nfor_node->node.nForNode.values[1] = endValue;
            nfor_node->node.nForNode.values[2] = stepValue;
            nfor_node->node.nForNode.statements = stments;

            return_node = nfor_node;
            break;
        }

        case TT_SEPARATOR:
        case TT_KWORD: {
            if (res->tokens++->tt == TT_SEPARATOR) EXPECTED(expect(res, TT_IDENTIFIER, "(IDENTIFIER) EXPECTED", &v, NULL))
            EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'in') EXPECTED", NULL, "in"))

            Node* target = expr(res);
            EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'repeat') EXPECTED", NULL, "repeat"))
            Node* stments = get_statements(res, END_TOKEN);

            Node* gfor_node = new_node(NT_GFOR);
            gfor_node->node.gForNode.identifiers[0] = i;
            gfor_node->node.gForNode.identifiers[1] = v;
            gfor_node->node.gForNode.target = target;
            gfor_node->node.gForNode.statements = stments;

            return_node = gfor_node;
            break;
        }

        default: SYNTAX_ERROR("(EQ:'=' || SEPARATOR:',') EXPECTED");
    }

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))
    return return_node;
}

Node* while_expr(ParseResult* res) {
    res->tokens++;

    Node* condition = expr(res);
    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'repeat') EXPECTED", NULL, "repeat"))
    Node* stments = get_statements(res, END_TOKEN);

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))
    Node* while_node = new_node(NT_WHILE);
    while_node->node.whileNode.condition = condition;
    while_node->node.whileNode.statements = stments;

    return while_node;
}

Node* repeat_expr(ParseResult* res) {
    res->tokens++;

    Node* stments = get_statements(res, WHILE_TOKEN);

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'while') EXPECTED", NULL, "while"))
    Node* condition = expr(res);

    Node* repeat_node = new_node(NT_REPEAT);
    repeat_node->node.whileNode.condition = condition;
    repeat_node->node.whileNode.statements = stments;

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))
    return repeat_node;
}

Node* import_expr(ParseResult* res) {
    res->tokens++;

    TValue module;
    TValue identifier = NIL;
    EXPECTED(expect(res, TT_STR, "(STR) EXPECTED", &module, NULL))

    if (match_keyword(*res->tokens++, "as")) EXPECTED(expect(res, TT_IDENTIFIER, "(IDENTIFIER) EXPECTED", &identifier, NULL))
    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))

    Node* import_node = new_node(NT_IMPORT);
    import_node->node.importNode.module = module;
    import_node->node.importNode.identifier = identifier;

    return import_node;
}

Node* func_expr(ParseResult* res) {
    res->tokens++;

    TValue func_name = NIL;
    if (res->tokens->tt == TT_IDENTIFIER) func_name = res->tokens++->value;

    EXPECTED(expect(res, TT_LPAR, "(LPAR : '(') EXPECTED", NULL, NULL))
    Node* func_node = new_node(NT_FUNCDEF);
    func_node->node.funcDefNode.name = func_name;
    func_node->node.funcDefNode.args.value = NIL;
    func_node->node.funcDefNode.args.next = NULL;

    if (res->tokens->tt == TT_IDENTIFIER) {
        func_node->node.funcDefNode.args.next = malloc(sizeof(struct TValueLinkedList));
        func_node->node.funcDefNode.args.next->value = res->tokens++->value;
        func_node->node.funcDefNode.args.next->next = NULL;
    }

    while (res->tokens->tt == TT_SEPARATOR) {
        res->tokens++;

        struct TValueLinkedList* new = malloc(sizeof(struct TValueLinkedList));
        EXPECTED(expect(res, TT_IDENTIFIER, "(IDENTIFIER) EXPECTED", &new->value, NULL))

        new->next =  func_node->node.funcDefNode.args.next;
        func_node->node.funcDefNode.args.next = new;
    }

    EXPECTED(expect(res, TT_RPAR, "(LPAR : ')') EXPECTED", NULL, NULL))
    Node* stments = get_statements(res, END_TOKEN);
    func_node->node.funcDefNode.statements = stments;

    EXPECTED(expect(res, TT_KWORD, "(KEYWORD:'end') EXPECTED", NULL, "end"))

    return func_node;
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
            EXPECTED(expect(res, TT_RPAR, "(RPAR : ')' ) EXPECTED", NULL, NULL))

            return_node->nt = nexpr->nt;
            return_node->node = nexpr->node;
            free(nexpr); res->tokens--;
            break;
        }

        case TT_KWORD : {
            if (match_keyword(*res->tokens, "func")) { free(return_node); return func_expr(res); }
        }

        default : {
            res->err = new_error("InvalidSyntaxError", "(INT | STR | OPERATOR) EXPECTED", 0, 0);
            free(return_node);
            return NULL;
        }
    }

    res->tokens++;
    return return_node;
}

Node* postfix(ParseResult* res) {
    Node* postfix_node = numerics(res);

    for (;;) {
        Node* op_node = postfix_op(res, postfix_node);
        if (!op_node) break; else postfix_node = op_node;
    }

    return postfix_node;
}

Node* power(ParseResult* res) { return bin_op(res, postfix, POWER_OPERATORS, POWER_OPERATORS_SIZE, factor); }

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
        EXPECTED(expect(res, TT_IDENTIFIER, "(IDENTIFIER) EXPECTED", &var_name, NULL))
        EXPECTED(expect(res, TT_EQ, "(EQ : '=') EXPECTED", NULL, NULL))

        Node* nexpr = expr(res);
        Node* return_node = new_node(NT_VAR_DECLARE);
        return_node->node.VarDeclareNode.var_name = var_name;
        return_node->node.VarDeclareNode.value = nexpr;

        return return_node;
    } else if (match_keyword(*res->tokens, "import")) return import_expr(res);

    Token* pret = res->tokens;
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
    else if (match_keyword(*res->tokens, "alloc")) return alloc_expr(res);
    else if (match_keyword(*res->tokens, "if")) return if_expr(res);
    else if (match_keyword(*res->tokens, "for")) return for_expr(res);
    else if (match_keyword(*res->tokens, "while")) return while_expr(res);
    else if (match_keyword(*res->tokens, "repeat")) return repeat_expr(res);

    return expr(res);
}

Node* statements(ParseResult* res) { return get_statements(res, EOF_TOKEN); }

Node* get_statements(ParseResult* res, Token stopToken) {
    Node* node = new_node(NT_STATEMENTS);
    NodeArray statementNodes = make_node_array();

    clean_newlines(&res->tokens, &res->adv_count);
    while (res->tokens->tt != TT_EOF && !match_token(*res->tokens, stopToken)) {
        Token* pre = res->tokens;
        Node* stmnt = statement(res);

        if (stmnt == NULL || res->err != NULL) { if (pre == res->tokens) res->err = NULL; break; }
        append_to_node_array(&statementNodes, stmnt);
        clean_newlines(&res->tokens, &res->adv_count);
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

static inline void expect(ParseResult* res, TokenType tt, char* expected, TValue* value, char* kword) {
    if (res->tokens->tt != tt || (kword && !match_keyword(*res->tokens, kword))) res->err = new_error("InvalidSyntaxError", expected, 0, 0);
    if (value != NULL) *value = res->tokens->value;
    *res->tokens++;
}
