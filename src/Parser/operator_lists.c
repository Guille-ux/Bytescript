#include "Parser/parser.h"
#include "Globals/string.h"


Token LOGICAL_OPERATORS[] = {
    (Token){ .tt = TT_AND, .value = NIL },          // 0
    (Token){ .tt = TT_OR, .value = NIL },           // 1
    (Token){ .tt = TT_BAND, .value = NIL },         // 2
    (Token){ .tt = TT_BOR, .value = NIL },          // 3
    (Token){ .tt = TT_BXOR, .value = NIL },         // 4
    (Token){ .tt = TT_KWORD, .value = NIL },        // 5
    (Token){ .tt = TT_KWORD, .value = NIL }         // 6
};

Token COMP_OPERATORS[] = {
    (Token){ .tt = TT_EEQ, .value = NIL },
    (Token){ .tt = TT_NEQ, .value = NIL },
    (Token){ .tt = TT_LT, .value = NIL },
    (Token){ .tt = TT_GT, .value = NIL },
    (Token){ .tt = TT_LTEQ, .value = NIL },
    (Token){ .tt = TT_GTEQ, .value = NIL }
};

Token ARITHMETIC_OPERATORS[] = {
    (Token){ .tt = TT_PLUS, .value = NIL },
    (Token){ .tt = TT_MINUS, .value = NIL },
    (Token){ .tt = TT_CONCAT, .value = NIL }
};

Token TERM_OPERATORS[] = {
    (Token){ .tt = TT_MUL, .value = NIL },
    (Token){ .tt = TT_DIV, .value = NIL },
    (Token){ .tt = TT_IDIV, .value = NIL },
    (Token){ .tt = TT_MOD, .value = NIL }
};

Token FACTOR_OPERATORS[] = {
    (Token){ .tt = TT_PLUS, .value = NIL },
    (Token){ .tt = TT_MINUS, .value = NIL }
};

Token POWER_OPERATORS[] = {
    (Token){ .tt = TT_POW, .value = NIL }
};


size_t LOGICAL_OPERATORS_SIZE = sizeof(LOGICAL_OPERATORS) / sizeof(LOGICAL_OPERATORS[0]);
size_t COMP_OPERATORS_SIZE = sizeof(COMP_OPERATORS) / sizeof(COMP_OPERATORS[0]);
size_t ARITHMETIC_OPERATORS_SIZE = sizeof(ARITHMETIC_OPERATORS) / sizeof(ARITHMETIC_OPERATORS[0]);
size_t TERM_OPERATORS_SIZE = sizeof(TERM_OPERATORS) / sizeof(TERM_OPERATORS[0]);
size_t FACTOR_OPERATORS_SIZE = sizeof(FACTOR_OPERATORS) / sizeof(FACTOR_OPERATORS[0]);
size_t POWER_OPERATORS_SIZE = sizeof(POWER_OPERATORS) / sizeof(POWER_OPERATORS[0]);
void init_operators() {
    LOGICAL_OPERATORS[5].value = make_fstring("and");
    LOGICAL_OPERATORS[6].value = make_fstring("or");
}