#include <stdio.h>
#include "bytescript.h"




void get_flags(int argc, char* argv[], char* byte_args[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            exit(1);
        } else {
            byte_args[filename] = argv[i];
        }
    }
}


char* read_file(FILE* file, size_t* out_size) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size == -1) FAILED_ALLOCATION_ERROR("Failed allocation while reading file");
    
    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer) FAILED_ALLOCATION_ERROR("Failed allocation of buffer while reading file");

    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';

    fclose(file);
    if (out_size) *out_size = size;
    return buffer;
}



int main(int argc, char* argv[]) {
    char* byte_args[argc];
    for (int i = 0; i < argc; i++) byte_args[i] = NULL;

    get_flags(argc, argv, byte_args);
    if (byte_args[filename] == NULL) call_error(new_error("Unspecified filename", "must specify filename", 0, 0), NULL);

    FILE* pFile = fopen(byte_args[filename], "r");

    if (pFile == NULL) {
        char details[snprintf(NULL, 0, "Could not find file : %s", byte_args[filename]) + 1];
        snprintf(details, sizeof(details), "Could not find file : %s", byte_args[filename]);
        call_error(new_error("Inexistant File", details, 0, 0), NULL);
    }


    size_t file_size = 0;
    char* buff = read_file(pFile, &file_size);

    initialize_globals();
    initialize_type_specifiers();
    format_string((TValue[2]){ make_fstring("idk %04.3s"), make_fstring("yes") }, 2);

    Token* tokens = make_tokens(buff);
    free(buff);

    Node* ast = parse(tokens, byte_args[filename]);

    TypesContext* type_context = new_resolve_context();
    resolve_type(ast, type_context);
    
    Scope* compile_context = new_scope();
    compile(ast, compile_context);

    size_t const_len = compile_context->const_pool.len;

    TValue main = close_context(compile_context);
    printf("main");
    full_disassemble(main);

    reset_table((TValue[1]){ TBL_TO_TVALUE(string_cache) }, 1);

    return 0;
}