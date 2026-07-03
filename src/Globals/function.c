#include "Globals/number.h"
#include "Values/oop.h"
#include "bytescript.h"

#include <stdio.h>

TValue Function_class;
TValue Function_obj_class;
void initialize_function_class() {
    TValue Function_obj_fields[] = {

    };

    TValue Function_fields[] = {

    };

    easy_map("src/Globals/maps/function_map.bs");
    Function_class = new_class(NULL, NULL, 0, sizeof(Function_fields) / sizeof(TValue));
    Function_obj_class = new_class(NULL, NULL, 0, sizeof(Function_obj_fields) / sizeof(TValue));
}
