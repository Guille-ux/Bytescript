#include "Values/oop.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Globals/globals.h"
#include "utils/utils.h"

TValue indexer;
TValue constructor;
TValue native_methods;
TValue native_methods_map;
void initialize_oop() {
    constructor = make_fstring("__new");
    indexer = make_fstring("__index");
    native_methods = new_table(NULL, 0);
    native_methods_map = new_table(NULL, 0);
    append_to_table(native_methods, constructor);
    append_to_table(native_methods, make_fstring("__add"));
    append_to_table(native_methods, make_fstring("__sub"));
    append_to_table(native_methods, make_fstring("__mul"));
    append_to_table(native_methods, make_fstring("__pow"));
    append_to_table(native_methods, make_fstring("__div"));
    append_to_table(native_methods, make_fstring("__idiv"));
    append_to_table(native_methods, make_fstring("__mod"));
    append_to_table(native_methods, make_fstring("__concat"));
    append_to_table(native_methods, make_fstring("__eq"));
    append_to_table(native_methods, make_fstring("__neq"));
    append_to_table(native_methods, make_fstring("__lt"));
    append_to_table(native_methods, make_fstring("__gt"));
    append_to_table(native_methods, make_fstring("__lteq"));
    append_to_table(native_methods, make_fstring("__gteq"));
    append_to_table(native_methods, make_fstring("__rand"));
    append_to_table(native_methods, make_fstring("__ror"));
    append_to_table(native_methods, make_fstring("__and"));
    append_to_table(native_methods, make_fstring("__or"));
    append_to_table(native_methods, make_fstring("__band"));
    append_to_table(native_methods, make_fstring("__bor"));
    append_to_table(native_methods, make_fstring("__lshift"));
    append_to_table(native_methods, make_fstring("__rshift"));
    append_to_table(native_methods, make_fstring("__unm"));
    append_to_table(native_methods, make_fstring("__unp"));
    append_to_table(native_methods, make_fstring("__not"));
    append_to_table(native_methods, make_fstring("__index"));

    TTable* native_table = AS_TABLE(native_methods);
    for (int i = 0; i < native_table->array.len; i++) {
        TValue undundered = undunder(native_table->array.array[i]);
        add_value_to_hashtable(native_methods_map, undundered, native_table->array.array[i]);
    }
}

TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count) {
    TClass* nclass = malloc(sizeof(TClass) + BSIZE(inherited_count) + BSIZE(field_count));
    nclass->inherited_count = inherited_count;
    if (inherited) memcpy(nclass->fields, inherited, BSIZE(inherited_count));
    if (fields) memcpy(nclass->fields + inherited_count, fields, BSIZE(field_count));

    return CLS_TO_TVALUE(nclass);
}

TValue new_object(TValue cls, CFunc constructor, size_t fields_count) {
    TObject* obj = malloc(sizeof(TObject) + BSIZE(fields_count));
    TValue tobj = OBJ_TO_TVALUE(obj);
    obj->class = AS_CLASS(cls);

    if (constructor) constructor(&tobj, 1);
    return tobj;

}

TClass* _as_class(TValue cls) { return AS_CLASS(cls); }
TObject* _as_object(TValue cls) { return AS_OBJECT(cls); }