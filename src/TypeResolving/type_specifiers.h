#ifndef TYPE_SPECIFIERS
#define TYPE_SPECIFIERS

#include "Values/bytev.h"
#include "Globals/string.h"

extern TValue varType;
extern TValue numType;
extern TValue strType;
extern TValue tableType;
extern TValue funcType;

void initialize_type_specifiers();
int is_native_type_specifier(TValue value);

#endif