#include "Values/oop.h"
#include "Values/bytescript_funcs.h"
#include "Globals/globals.h"

TValue Function_class;
void initialize_function_class() {
    TValue Function_fields[] = {

    };

    clean_node(easy_map("function_map.bs"));
    Function_class = new_class(NULL, NULL, 0, sizeof(Function_fields) / sizeof(TValue));

    add_global(Function_class, make_fstring("Function"));
}
