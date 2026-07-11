#include "Globals/Module/module.h"
#include "Globals/string.h"
#include "Globals/globals.h"

TValue loaded_modules;
TValue Module_class;
void initialize_module_class() {
    loaded_modules = new_table(NULL, 0);

    TValue Module_fields[] = {
        loaded_modules
    };

    easy_map("src/Globals/maps/module_map.bs");
    Module_class = new_class(NULL, Module_fields, 0, sizeof(Module_fields) / sizeof(Module_fields[0]));
    
    add_global(Module_class, make_fstring("Module"));
}

