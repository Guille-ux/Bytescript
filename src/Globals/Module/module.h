#ifndef MODULES_H
#define MODULES_H

#include "Values/bytev.h"
#include "Globals/globals.h"

extern TValue global_idxs_module;
extern TValue loaded_modules;
extern TValue loaded_returns;
extern TValue Module_class;
void initialize_module_class();

int module_load(TValue value, TValue name);
int module_get(TValue name);
int module_get_global_idx(TValue name);

#endif