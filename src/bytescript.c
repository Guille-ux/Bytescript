#include "Values/bytescript_funcs.h"
#include "Values/oop.h"
#include "Globals/globals.h"
#include "TypeResolving/type_specifiers.h"
#include "bytescript.h"

void get_flags(int argc, char* argv[], char* byte_args[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            exit(1);
        } else {
            byte_args[FLAG_FILENAME] = argv[i];
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
    if (byte_args[FLAG_FILENAME] == NULL) call_error(new_failed_allocation_error, "must specify filename");

    TValue main = easy_compile(byte_args[FLAG_FILENAME]);
    TValue return_value = vm_run(main);

    return 0;
}
