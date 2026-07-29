#include "Globals/number.h"
#include "Values/oop.h"
#include "Values/bytescript_funcs.h"
#include "Globals/globals.h"

TValue Number_class;
void initialize_number_class() {
    TValue Number_fields[] = {

    };

    clean_node(easy_map("number_map.bs"));
    Number_class = new_class(NULL, NULL, 0, sizeof(Number_fields) / sizeof(TValue));

    add_global(Number_class, make_fstring("Number"));
}
