#include "Globals/number.h"
#include "Values/oop.h"
#include "bytescript.h"

TValue Number_class;
TValue Number_obj_class;
void initialize_number_class() {
    TValue Number_obj_fields[] = {

    };

    TValue Number_fields[] = {

    };

    easy_map("src/Globals/maps/number_map.bs");
    Number_class = new_class(NULL, NULL, 0, sizeof(Number_fields) / sizeof(TValue));
    Number_obj_class = new_class(NULL, NULL, 0, sizeof(Number_obj_fields) / sizeof(TValue));
}
