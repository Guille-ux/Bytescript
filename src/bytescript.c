#include <stdio.h>
#include "bytescript.h"
#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "TypeResolving/type_resolving.h"
#include "Compiler/constant_folding.h"
#include "Compiler/class_mapping.h"
#include "Compiler/compiler.h"
#include "Compiler/ast_cleaner.h"
#include "VM/vm.h"
#include "utils/disassembler.h"
#include "Globals/IO/IO.h"
#include "Globals/IO/File.h"



void get_flags(int argc, char* argv[], char* byte_args[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            exit(1);
        } else {
            byte_args[filename] = argv[i];
        }
    }
}

int main(int argc, char* argv[]) {
    intialize_dependencies();
    initialize_type_specifiers();
    initialize_oop();
    initialize_globals();

    char* byte_args[argc];
    for (int i = 0; i < argc; i++) byte_args[i] = NULL;

    get_flags(argc, argv, byte_args);
    if (byte_args[filename] == NULL) call_error(new_failed_allocation_error, "must specify filename");

    TValue main = easy_compile(byte_args[filename]);
    TValue return_value = vm_run(main);
    //disassemble(&main, 1);

    reset_table((TValue[1]){ TBL_TO_TVALUE(string_cache) }, 1);
    return 0;
}


void intialize_dependencies() {
    type_namespace = new_table(NULL, 0);
    global_types = new_table(NULL, 0);
    global_functypes = new_table(NULL, 0);
    user_types = new_table(NULL, 0);
    string_cache = AS_TABLE(new_table(NULL, 0));
}

Node* easy_parse(char bsname[]) {
    TValue file = open_IO((TValue[2]){ NIL, make_fstring(bsname) }, 2);
    TValue srccode = read_file(&file, 1);
    TStr* srcode_str = AS_STRING(srccode);

    Token* tokens = make_tokens(srcode_str->str);
    tfree(&srccode, 1);

    Node* ast = parse(tokens, bsname);

    TypesContext* type_context = new_resolve_context();
    resolve_type(ast, type_context);

    fold(ast);
    return ast;
}

Node* easy_map(char bsname[]) {
    Node* ast = easy_parse(bsname);
    classmap(ast);

    return ast;
}

TValue easy_compile(char bsname[]) {
    Node* ast = easy_map(bsname);
    
    Scope* compile_context = new_scope();
    compile(ast, compile_context);

    return close_context(compile_context);
}
