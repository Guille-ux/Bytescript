#ifndef BYTESCRIPT
#define BYTESCRIPT

#include "Values/bytev.h"
#include "Parser/nodes.h"

typedef enum {
    FLAG_FILENAME
} FlagsIndex;

void intialize_dependencies();
Node* simple_parse(char filename[]);
Node* easy_parse(char filename[]);
Node* easy_map(char filename[]);
TValue easy_compile(char filename[]);
TValue exec(TValue* argv, size_t argc);

#endif