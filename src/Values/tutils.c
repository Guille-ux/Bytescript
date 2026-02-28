#include "Values/tutils.h"
#include "Error/error.h"


TValue to_return_value(TValue returnv[], size_t returnc) {
    if (returnc == 1) return *returnv;
    if (returnc == 0) return NIL;
    return new_table(returnv, returnc);
}

TValue potential_shrink(TTable* table) {
    if (table->len <= table->capacity / 2) {
        shrink_table(&table, 1);
    }
}

TValue make_string(char* str) {
    size_t stlen = strlen(str);

    TStr* nTStr = malloc(sizeof(TStr) + stlen + 1);
    nTStr->len = stlen;
    strcpy(nTStr->str, str);

    return STR_TO_TVALUE(nTStr);
}

TTable* shift_table(TTable* table, size_t dead_values) {
    for (int i = 0; i < table->len + dead_values; i++) {
        if (IS_NIL(table->array[i])) {
            table->array[i] = table->array[i + 1];
            table->array[i + 1] = NIL;
        }
    }

    return table;
}

TTable* shift_table_right(TTable* table, size_t idx) {
    for (int i = table->len; i > idx; i--) {
        table->array[i] = table->array[i - 1];
    }

    return table;
}

char* read_string() {
    size_t size = 4;
    size_t count = 0;
    char* str = malloc(size + 1);
    if (!str) return NULL;

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (count >= size) {
            size *= 2;
            char* new_str = realloc(str, size + 1);
            if (!new_str) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation while reading stdout");

            str = new_str;
        }

        str[count++] = (char)c;
    }

    str[count] = '\0';
    return str;
}