#ifndef OOP_H
#define OOP_H

#include "Values/bytev.h"

extern TValue indexer;
extern TValue constructor;
extern TValue native_methods;
extern TValue native_methods_map;

void initialize_oop();
TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count);
TValue new_object(TValue cls, CFunc constructor, size_t fields_count);
TValue global_class_from_fields(TValue fields[][2], size_t fields_size, char name[]);

static inline TValue get_class_field(TValue class, int idx) {
    TClass* tclass = AS_CLASS(class);
    return tclass->fields[tclass->inherited_count + idx];
}

#endif