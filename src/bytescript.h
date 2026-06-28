#ifndef BYTESCRIPT
#define BYTESCRIPT

#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "TypeResolving/type_resolving.h"
#include "Compiler/constant_folding.h"
#include "Compiler/compiler.h"
#include "Compiler/ast_cleaner.h"
#include "utils/disassembler.h"
#include "Values/bytev.h"


typedef enum {
    filename
} FlagsIndex;


#endif