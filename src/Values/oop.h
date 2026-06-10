#ifndef OOP_H
#define OOP_H

#include "Values/bytev.h"


TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count);
TValue new_object(TClass* clss);


#endif