#include "Globals/string.h"
#include "Globals/table.h"
#include <stdbool.h>


const TTable* string_cache = NULL;


inline TValue cache_string(TStr* str) {
    TValue tstr = STR_TO_TVALUE(str);

    if (string_cache == NULL) string_cache = AS_TABLE(new_table(NULL, 0));
    else {
        TValue cached =  get_value_from_hashtable(TBL_TO_TVALUE(string_cache), tstr);
       // printf("%d\n", IS_NIL(cached));

        if (!IS_NIL(cached)) {
            free(str);
            return cached;
        }
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


TValue compare_string(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_STRING(value)) TYPE_ERROR("function <String.compare> take type <String> as arguments");

        TTable* ptr = AS_TABLE(value);
        free(ptr->array.array);
        free(ptr->hash_table.array);
        free(ptr);
    }

    return NIL;
}