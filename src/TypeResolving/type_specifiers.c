#include "TypeResolving/type_specifiers.h"
#include "bytescript.h"
#include "Globals/table.h"

TValue varType;
TValue numType;
TValue strType;
TValue tableType;
TValue FunctionType;
TValue NumberType;
TValue StringType;
TValue TableType;

TValue* typeTable[] = {
    &varType,
    &numType,
    &NumberType,
    &strType,
    &StringType,
    &tableType,
    &TableType,
    &FunctionType,
};

TValue operation_methods;
TValue unary_operation_methods;
TCDataType MethodDescriptor_Type = { .name = "MethodDescriptor" }, *MethodDescriptorType = &MethodDescriptor_Type;
TCDataType ClassDescriptor_Type = { .name = "ClassDescriptor" }, *ClassDescriptorType = &ClassDescriptor_Type;

void initialize_type_specifiers() {
    varType     = make_fstring("var");
    numType     = make_fstring("num");
    strType     = make_fstring("str");
    tableType   = make_fstring("table");
    FunctionType= make_fstring("Function");
    NumberType  = make_fstring("Number");
    StringType  = make_fstring("String");
    TableType   = make_fstring("Table");

    return initialize_operation_methods();
}

void initialize_operation_methods() {
    unary_operation_methods = new_table(NULL, 0);
    add_value_to_hashtable(unary_operation_methods, NUM_AS_TVALUE(TT_MINUS), make_fstring("__unm"));
    add_value_to_hashtable(unary_operation_methods, NUM_AS_TVALUE(TT_PLUS), make_fstring("__unp"));
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

int is_native_type_specifier(TValue* value) {
    if (EQUAL(*value, numType)) *value = NumberType;
    else if (EQUAL(*value, strType)) *value = StringType;
    else if (EQUAL(*value, tableType)) *value = TableType;

    for (int i = 0; i < sizeof(typeTable) / sizeof(typeTable[0]); i++) if (EQUAL(*value, *typeTable[i])) return 1;
    return 0;
}

TValue make_method_descriptor(TValue arg_types[], TValue return_type, size_t argc) {
    MethodDescriptor* res_descriptor = malloc(sizeof(MethodDescriptor));
    res_descriptor->cdata_type = MethodDescriptorType;
    res_descriptor->arg_types = new_table(arg_types, argc);
    res_descriptor->return_type = return_type;

    return CDT_TO_TVALUE(res_descriptor);
}

TValue make_class_descriptor(TValue func_map, TValue id_map) {
    ClassDescriptor* res_descriptor = malloc(sizeof(ClassDescriptor));
    res_descriptor->func_namespace = IS_NIL(func_map) ? AS_TABLE(new_table(NULL, 0)) : AS_TABLE(func_map);
    res_descriptor->id_namespace   = IS_NIL(id_map) ? AS_TABLE(new_table(NULL, 0)) : AS_TABLE(id_map);
    res_descriptor->cdata_type = ClassDescriptorType;

    return CDT_TO_TVALUE(res_descriptor);
}

MethodDescriptor* as_method_descriptor(TValue value) {
    TCData* data = AS_CDATA(value);
    if (*data != MethodDescriptorType) call_error(new_type_error, "Failed to cast tvalue into function descriptor");

    return (MethodDescriptor*)data;
}

ClassDescriptor* as_class_descriptor(TValue value) {
    TCData* data = AS_CDATA(value);
    if (*data != ClassDescriptorType) call_error(new_type_error, "Failed to cast tvalue into class descriptor");

    return (ClassDescriptor*)data;
}


TValue right_operation_method(BTokenType op) {
    TValue op_method = get_value_from_hashtable(operation_methods, NUM_AS_TVALUE(op));
    TStr* ptr = AS_STRING(op_method);

    char buffer[ptr->len + 2];
    buffer[0] = '_'; buffer[1] = '_'; buffer[2] = 'r';

    strcpy(buffer + 3, ptr->str + 2);
    return make_fstring(buffer);
}