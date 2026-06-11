#include "Globals/string.h"
#include "Globals/table.h"
#include "Values/oop.h"

#include <stdbool.h>


const TTable* string_cache = NULL;


TValue initialize_string_class() {
    string_cache = AS_TABLE(new_table(NULL, 0));

    TValue String_methods[][2] = {
        NEW_METHOD(compare, string),
        NEW_METHOD(concat, string),
        NEW_METHOD(to_string, string)
    };

    size_t fields_size = sizeof(String_methods) / sizeof(String_methods[0]);
    TValue String_map = new_table(NULL, 0);
    TValue String_fields[fields_size];

    for (int i = 0; i < fields_size; i++) {
        String_fields[i] = String_methods[i][1];
        add_value_to_hashtable(String_map, String_methods[i][0], NUM_AS_TVALUE(i));
    }

    TValue String_class = new_class(NULL, String_fields, 0, fields_size);
    TValue String_namespace = new_namespace_entry(String_class, AS_TABLE(String_map));

    add_value_to_hashtable(TBL_TO_TVALUE(globals_namespace), make_fstring("String"), String_namespace);
    return String_class;
}


TValue compare_string(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_STRING(value)) TYPE_ERROR("function <String.compare> take type <String> as arguments");

        TTable* ptr = AS_TABLE(value);  // ??????????????????
        free(ptr->array.array);
        free(ptr->hash_table.array);
        free(ptr);
    }

    return NIL;
}

TValue concat_string(TValue* argv, size_t argc) {
    if (argc != 2) INVALID_ARGS_ERROR("function <String.concat> take 2 arguments");
    if (!IS_STRING(argv[0]) || !IS_STRING(argv[1])) TYPE_ERROR("function <String.concat> take type <String> as arguments");

    TStr* v0 = AS_STRING(argv[0]);
    TStr* v1 = AS_STRING(argv[1]);

    TStr* new = malloc(sizeof(TStr) + v0->len + v1->len + 1);
    strcpy(new->str, v0->str);
    strcat(new->str, v1->str);
    new->len = sizeof(TStr) + v0->len + v1->len;
    new->str[new->len] = '\0';

    return cache_string(new);
}

TValue to_string_string(TValue* argv, size_t argc) {
    TValue str1 = NIL;

    for (int i = 0; i < argc; i++) {
        TValue v = argv[i];
        TValue object;
        void* ptr;

        switch (TYPE(v)) {
            case STRING: {
                str1 = IS_NIL(str1) ? v : concat_string(( TValue[2] ){ str1, v }, 2);
                break;
            }

            case TABLE:
            case FUNC: {
                object = TYPE(v) == FUNC ? make_fstring("function at") : make_fstring("table at");;
                void* ptr = (void*)PAYLOAD(v);


                int needed = snprintf(NULL, 0, "<%p>", ptr);
                char buffer[needed + 1];
                snprintf(buffer, needed + 1, "<%p>", ptr);

                TValue str = make_fstring(buffer);
                str = concat_string(( TValue[2] ){ object, str }, 2);

                str1 = IS_NIL(str1) ? str : concat_string(( TValue[2] ){ str1, str }, 2);
                break;
            }

            default:
                int needed = snprintf(NULL, 0, "%.15g", AS_NUM(v));
                char buffer[needed + 1];
                snprintf(buffer, needed + 1, "%.15g", AS_NUM(v));

                str1 = IS_NIL(str1) ? make_fstring(buffer) : concat_string(( TValue[2] ){ str1, make_fstring(buffer) }, 2);
                break;
        }
    }

    return to_return_value(&str1, 1);
}


inline TValue cache_string(TStr* str) {
    TValue tstr = STR_TO_TVALUE(str);
    TValue cached =  get_value_from_hashtable(TBL_TO_TVALUE(string_cache), tstr);

    if (!IS_NIL(cached)) {
        free(str);
        return cached;
    }

    add_value_to_hashtable(TBL_TO_TVALUE(string_cache), tstr, tstr);
    return tstr;
}

inline TValue make_string(char* string, size_t len) {
    TStr* str = malloc(sizeof(TStr) + len + 1);
    strncpy(str->str, string, len);

    str->str[len] = '\0';
    str->len = len;
    return cache_string(str);
}

inline TValue make_fstring(char* str) {
    return make_string(str, strlen(str));
}
