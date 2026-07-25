#include "Values/bytescript_funcs.h"
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

void intialize_dependencies() {
    type_namespace = new_table(NULL, 0);
    global_types = new_table(NULL, 0);
    global_functypes = new_table(NULL, 0);
    user_types = new_table(NULL, 0);
    string_cache = AS_TABLE(new_table(NULL, 0));
}

Node* simple_parse(char filename[]) {
    TValue file_path = import_path_finder(make_fstring(filename));

    TValue file = open_IO((TValue[1]){ file_path }, 1);
    TValue srccode = read_file(&file, 1);
    TStr* srcode_str = AS_STRING(srccode);

    Token* tokens = make_tokens(srcode_str->str);
    tfree(&srccode, 1);
    close_file(&file, 1);

    return parse(tokens, AS_STRING(file_path)->str);
}

Node* easy_parse(char filename[]) {
    Node* ast = simple_parse(filename);

    TypesContext* type_context = new_resolve_context();
    resolve_type(ast, type_context);
    close_resolve_context(type_context);

    fold(ast);
    return ast;
}

Node* easy_map(char filename[]) {
    Node* ast = easy_parse(filename);
    classmap(ast);

    return ast;
}

TValue easy_compile(char filename[]) {
    Node* ast = easy_map(filename);
    
    Scope* compile_context = new_scope();
    compile(ast, compile_context);

    return close_context(compile_context);
}

TValue exec(TValue* argv, size_t argc) {
    MIN_ARGC(1);

    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue source_code = argv[i];
        if (TYPE(source_code) != STRING) call_error(new_type_error, "function <exec> take type <String> as args");

        TStr* srcode_str = AS_STRING(source_code);

        Token* tokens = make_tokens(srcode_str->str);
        Node* ast = parse(tokens, "Anonymous");

        TypesContext* type_context = new_resolve_context();
        resolve_type(ast, type_context);

        fold(ast);
        classmap(ast);

        Scope* compile_context = new_scope();
        compile(ast, compile_context);

        TValue func = close_context(compile_context);
        returnv[i] = vm_run(func);
    }

    return to_return_value(returnv, returnc);
}