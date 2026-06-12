#include "TypeResolving/type_specifiers.h"

TValue varType;
TValue numType;
TValue strType;
TValue tableType;
TValue funcType;

void initialize_type_specifiers() {
    varType     = make_fstring("var");
    numType     = make_fstring("num");
    strType     = make_fstring("str");
    tableType   = make_fstring("table");
    funcType    = make_fstring("func");
}

int is_native_type_specifier(TValue value) {
    return  EQUAL(value, varType)   ||
            EQUAL(value, numType)   ||
            EQUAL(value, strType)   ||
            EQUAL(value, tableType) ||
            EQUAL(value, funcType);
}