#ifndef OOP_H
#define OOP_H

#include "Values/bytev.h"


TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count);
TValue new_object(TClass* clss);

static inline TValue get_class_field(TValue class, int idx) {
    TClass* tclass = AS_CLASS(class);
    return tclass->fields[tclass->inherited_count + idx];
}

#endif