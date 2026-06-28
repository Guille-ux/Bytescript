#include "Values/oop.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Globals/globals.h"


TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count) {
    TClass* nclass = malloc(sizeof(TClass) + BSIZE(inherited_count) + BSIZE(field_count));
    nclass->inherited_count = inherited_count;
    memcpy(nclass->fields, inherited, BSIZE(inherited_count));
    memcpy(nclass->fields + inherited_count, fields, BSIZE(inherited_count));

    return CLS_TO_TVALUE(nclass);
}

TValue new_object(TClass* clss) {

}

TValue global_class_from_fields(TValue fields[][2], size_t fields_size, char name[]) {
    TValue map = new_table(NULL, 0);
    TValue fieldvalues[fields_size];

    append_methods_to_map(map, fields, fieldvalues, fields_size);

    TValue class = new_class(NULL, fieldvalues, 0, fields_size);
    TValue namespace = new_namespace_entry(class, AS_TABLE(map));

    add_value_to_hashtable(TBL_TO_TVALUE(globals_namespace), make_fstring(name), namespace);
    return class;
}
