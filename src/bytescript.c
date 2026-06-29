#include <stdio.h>
#include "bytescript.h"
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
    initialize_globals();
    initialize_type_specifiers();

    char* byte_args[argc];
    for (int i = 0; i < argc; i++) byte_args[i] = NULL;

    get_flags(argc, argv, byte_args);
    if (byte_args[filename] == NULL) call_error(new_failed_allocation_error, "must specify filename");

    TValue file = open_IO((TValue[2]){ NIL, make_fstring(byte_args[filename]) }, 2);
    TValue srccode = read_file(&file, 1);
    TStr* srcode_str = AS_STRING(srccode);

    printf("lex\n");
    Token* tokens = make_tokens(srcode_str->str);
    tfree(&srccode, 1);

    printf("parse\n");
    Node* ast = parse(tokens, byte_args[filename]);

    printf("typu\n");
    TypesContext* type_context = new_resolve_context();
    resolve_type(ast, type_context);

    printf("fold\n");
    fold(ast);
    
    printf("compile\n");
    Scope* compile_context = new_scope();
    compile(ast, compile_context);

    TValue main = close_context(compile_context);
    printf("main");
    full_disassemble(main);

    reset_table((TValue[1]){ TBL_TO_TVALUE(string_cache) }, 1);
    return 0;
}