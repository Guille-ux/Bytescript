#ifndef TYPE_SPECIFIERS
#define TYPE_SPECIFIERS

#include "Values/bytev.h"
#include "Globals/string.h"

extern TValue varType;
extern TValue numType;
extern TValue strType;
extern TValue tableType;
extern TValue funcType;
extern TValue NumberType;
extern TValue StringType;
extern TValue TableType;

extern TValue num_map;
extern TValue str_map;
extern TValue table_map;
extern TValue func_map;

extern TValue* typeTable[][2];

typedef struct {
    CDATA_HEADER;
    TValue arg_types;
    TValue return_type;
} MethodDescriptor;

extern TValue operation_methods;
extern TValue unary_operation_methods;
extern TCDataType MethodDescriptor_Type, *MethodDescriptorType;

void initialize_type_specifiers();
void initialize_operation_methods();
int is_native_type_specifier(TValue* value);
TValue make_method_descriptor(TValue arg_types[], TValue return_type, size_t argc);

#endif