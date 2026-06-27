#include "TypeResolving/type_specifiers.h"
#include "Globals/table.h"

TValue varType;
TValue numType;
TValue strType;
TValue tableType;
TValue funcType;

TValue num_map;
TValue str_map;
TValue table_map;
TValue func_map;

TValue operation_methods;
TValue unary_operation_methods;
TCDataType MethodDescriptor_Type = { .name = "MethodDescriptor" }, *MethodDescriptorType = &MethodDescriptor_Type;

void initialize_type_specifiers() {
    varType     = make_fstring("var");
    numType     = make_fstring("num");
    strType     = make_fstring("str");
    tableType   = make_fstring("table");
    funcType    = make_fstring("func");

    num_map = new_table(NULL, 0);
    TValue num_descriptor = make_method_descriptor((TValue[1]){ numType }, numType, 1);
    add_value_to_hashtable(num_map, make_fstring("__add"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__sub"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__mul"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__pow"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__div"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__idiv"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__mod"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__eq"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__neq"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__lt"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__gt"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__lteq"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__gteq"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__rand"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__ror"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__and"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__or"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__band"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__bor"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__lshift"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__rshift"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__unm"), num_descriptor);
    add_value_to_hashtable(num_map, make_fstring("__not"), num_descriptor);

    str_map = new_table(NULL, 0);
    TValue str_descriptor = make_method_descriptor((TValue[1]){ strType }, strType, 1);
    add_value_to_hashtable(num_map, make_fstring("__concat"), str_descriptor);

    table_map = new_table(NULL, 0);
    add_value_to_hashtable(table_map, make_fstring("__iter"), make_method_descriptor((TValue[2]){ varType, varType }, varType, 2 ));

    func_map = new_table(NULL, 0);

    unary_operation_methods = new_table(NULL, 0);
    add_value_to_hashtable(unary_operation_methods, NUM_AS_TVALUE(TT_MINUS), make_fstring("__unm"));
    add_value_to_hashtable(unary_operation_methods, NUM_AS_TVALUE(TT_NOT), make_fstring("__not"));

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
}

int is_native_type_specifier(TValue value) {
    return  EQUAL(value, varType)   ||
            EQUAL(value, numType)   ||
            EQUAL(value, strType)   ||
            EQUAL(value, tableType) ||
            EQUAL(value, funcType);
}

TValue make_method_descriptor(TValue arg_types[], TValue return_type, size_t argc) {
    MethodDescriptor* res_descriptor = malloc(sizeof(MethodDescriptor));
    res_descriptor->cdata_type = MethodDescriptorType;
    res_descriptor->arg_types = new_table(arg_types, argc);
    res_descriptor->return_type = return_type;

    return CDT_TO_TVALUE(res_descriptor);
}