#include "Values/oop.h"
#include "Values/bytescript_funcs.h"
#include "Globals/globals.h"

TValue CData_class;
void initialize_cdata_class() {
    TValue CData_fields[] = {

    };

    clean_node(easy_map("cdata_map.bs"));
    CData_class = new_class(NULL, NULL, 0, sizeof(CData_fields) / sizeof(TValue));

    add_global(CData_class, make_fstring("CData"));
}
