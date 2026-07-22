#include "Globals/Module/module.h"
#include "Globals/string.h"
#include "Globals/globals.h"
#include "Values/bytescript_funcs.h"

TValue loaded_modules;
TValue loaded_returns;
TValue Module_class;
void initialize_module_class() {
    loaded_modules = new_table(NULL, 0);
    loaded_returns = new_table(NULL, 0);

    TValue Module_fields[] = {
        loaded_modules,
        loaded_returns
    };

    easy_map("module_map.bs");
    Module_class = new_class(NULL, Module_fields, 0, sizeof(Module_fields) / sizeof(Module_fields[0]));
    
    add_global(Module_class, make_fstring("Module"));
}


int module_load(TValue value, TValue name) {
    append_to_table(loaded_modules, value);
    int idx = len_table(&loaded_modules, 1).num - 1;

    add_value_to_hashtable(loaded_modules, name, NUM_AS_TVALUE(idx));
    return idx;
}

int module_load_returns(TValue value, TValue name) {
    append_to_table(loaded_returns, value);
    int idx = len_table(&loaded_returns, 1).num - 1;

    add_value_to_hashtable(loaded_returns, name, NUM_AS_TVALUE(idx));
    return idx;
}

int module_get(TValue name) {
    return get_from_table(loaded_modules, get_value_from_hashtable(loaded_modules, name)).num;
}

int module_get_return(TValue name) {
    return get_from_table(loaded_returns, get_value_from_hashtable(loaded_returns, name)).num;
}