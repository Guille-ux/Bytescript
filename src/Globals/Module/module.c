#include "Globals/Module/module.h"
#include "Globals/string.h"
#include "Globals/globals.h"
#include "bytescript.h"

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


int module_load(TValue value, TValue name) {
    append_to_table(loaded_modules, value);
    int idx = len_table(&loaded_modules, 1).num - 1;

    add_value_to_hashtable(loaded_modules, name, NUM_AS_TVALUE(idx));
    return idx;
}

int module_get(TValue name) {
    return get_from_table(loaded_modules, get_value_from_hashtable(loaded_modules, name)).num;
}