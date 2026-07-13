#include "utils/utils.h"
#include "Globals/table.h"

#include <stdio.h>
#include <ctype.h>


TValue shift_tvalue_array(TValue arr[], size_t size, int start) {
    for (int i = start; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

uint8_t shift_uint8_array(uint8_t arr[], size_t size, int start) {
    for (int i = start; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

char* snake_to_camel_case(char* snaked, size_t len) {
    for (int i = 0; i < len; i++) {
        if (snaked[i] == '_') {
            char* new = snake_to_camel_case(snaked + i + 1, len - i - 1);
            char buffer[strlen(new) + 1];

            strcpy(buffer, new);
            strcpy(snaked + i, buffer);

            snaked[i] = toupper(snaked[i]);
            snaked[len - 1] = '\0';
        }
    }

    return snaked;
}

int has_modifier(TValue tbl, char modif[]) { return in_table(tbl, make_fstring(modif)); }

TValue dunder(TValue str) {
    TValue dunder_str = make_fstring("__");
    return concat_string((TValue[2]){ dunder_str, str }, 2);
}

TValue undunder(TValue str) {
    return make_fstring(AS_STRING(str)->str + 2);
}

TValue is_dunder(TValue tstr) {
    TStr* str = AS_STRING(tstr);

    if (str->str[0] == '_' && str->str[1] == '_') return TRUE_TVALUE;
    return FALSE_TVALUE;
}

void counting_sort(TValue table) {
    // :0 sorting algorithm

    TTable* ptr = AS_TABLE(table);
    Array arr = ptr->array;
    size_t len = ptr->array.len;
    uint8_t counts[256] = {0};

    for (int i = 0; i < len; i++) counts[(int)ptr->array.array[i].num]++;
    for (int i = 0; i < 256; i++) {
        uint8_t count = counts[i];
        for (int j = 0; j < count; j++) {
            arr.array[j] = NUM_AS_TVALUE(i);
        }

        arr.array += count;
    }
}

int has_ext(TValue path, char ext[]) {
    TStr* str = AS_STRING(path);
    size_t ext_len = strlen(ext);

    if (ext_len > str->len) return 0;
    return !strcmp(str->str + str->len - ext_len, ext);
}