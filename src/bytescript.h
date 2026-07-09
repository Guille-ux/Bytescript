#ifndef BYTESCRIPT
#define BYTESCRIPT

#include "Values/bytev.h"
#include "Parser/nodes.h"

typedef enum {
    filename
} FlagsIndex;

void intialize_dependencies();
Node* easy_parse(char bsname[]);
Node* easy_map(char bsname[]);
TValue easy_compile(char bsname[]);
TValue exec(TValue* argv, size_t argc);

#endif