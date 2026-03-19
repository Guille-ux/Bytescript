#include "Globals/string.h"
#include "Globals/table.h"
#include <stdbool.h>


const TTable* string_cache = NULL;


inline TValue cache_string(char* string, size_t len) {
    struct {
        size_t len;
        char str[len];
    } fake_pre_cache;

    fake_pre_cache.len = len;
    memcpy(fake_pre_cache.str, string, len);
    TStr* pre_cache = (TStr*)(&fake_pre_cache);

    if (string_cache == NULL) { string_cache = AS_TABLE(new_table(NULL, 0)); return NIL; }
    else return get_value_from_hashtable(TBL_TO_TVALUE(string_cache), STR_TO_TVALUE(pre_cache));
}

inline TValue make_string(char* string, size_t len) {
    TValue cached = cache_string(string, len);
    if (!IS_NIL(cached)) return cached;

    TStr* str = malloc(sizeof(TStr) + len + 1);
    strncpy(str->str, string, len);

    str->str[len] = '\0';
    str->len = len;

    return STR_TO_TVALUE(str);
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