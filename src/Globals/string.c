#include "Globals/string.h"
#include "Globals/table.h"
#include <stdbool.h>


const TTable* string_cache = NULL;


inline TValue cache_string(TStr* str) {
    TValue tstr = STR_TO_TVALUE(str);

    if (string_cache == NULL) string_cache = AS_TABLE(new_table(NULL, 0));
    else {
        TValue cached =  get_value_from_hashtable(TBL_TO_TVALUE(string_cache), tstr);

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