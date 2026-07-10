#include "Globals/Module/module.h"
#include "Globals/string.h"
#include "Globals/globals.h"

TValue Module_class;
void initialize_module_class() {
    TValue Module_fields[] = {

    };

    easy_map("src/Globals/maps/module_map.bs");
    Module_class = new_class(NULL, NULL, 0, sizeof(Module_fields) / sizeof(Module_fields[0]));
    
    add_global(Module_class, make_fstring("Module"));
    initialize_file_class();
}

