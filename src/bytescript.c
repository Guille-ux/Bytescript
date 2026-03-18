#include <stdio.h>

#include "bytescript.h"
#include "Lexer/lexer.h"

#include "Error/error.h"
#include "Values/bytev.h"




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
    size_t size = ftell(file);

    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer) FAILED_ALLOCATION_ERROR("failed allocation of buffer while reading file");

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

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


    size_t tok_len = 0;
    Token* tokens = make_tokens(buff, file_size, &tok_len);
    print_tokens(tokens, tok_len);


    fclose(pFile);
    return 0;
}