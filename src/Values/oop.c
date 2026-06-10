#include "Values/oop.h"


TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count) {
    TClass* nclass = malloc(sizeof(TClass) + BSIZE(inherited_count) + BSIZE(field_count));
    nclass->inherited_count = inherited_count;
    memcpy(nclass->fields, inherited, BSIZE(inherited_count));
    memcpy(nclass->fields + inherited_count, fields, BSIZE(inherited_count));

    return CLS_TO_TVALUE(nclass);
}

TValue new_object(TClass* clss) {

}