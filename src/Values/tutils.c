#include "tutils.h"


TValue to_return_value(TValue returnv[], size_t returnlen) {
    if (returnlen == 1) return *returnv;
    if (returnlen == 0) return NUM_AS_TVALUE((double)(0));

    TValue* newarray = malloc(sizeof(TValue) * returnlen);
    TTable* newtab = malloc(sizeof(TTable));
    if (newarray == NULL || newtab == NULL) exit(1);
    newtab->array = newarray;
    newtab->capacity = returnlen;
    newtab->len = returnlen;
    
    for (int i = 0; i < returnlen; i++) {
        newarray[i] = returnv[i];
    }
    
    uint64_t ptr = (uint64_t)(newtab);
    uint64_t value = (TABLE | ptr);
    return NAN_AS_TVALUE(value);
}

TTable* shift_table(TTable* table, size_t dead_values) {
    for (int i = 0; i <= table->len + dead_values; i++) {
        if (AS_NUM(table->array[i]) == 0) {
            table->array[i] = table->array[i + 1];
            table->array[i + 1] = NUM_AS_TVALUE((double)(0));
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
    char* str = malloc(size + 1);
    if (!str) return NULL;

    size_t count = 0;
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        if (count >= size) {
            size *= 2;
            char *new_str = realloc(str, size + 1);

            if (!new_str) {
                free(str);
                return NULL;
            }
            
            str = new_str;
        }

        str[count++] = (char)c;
    }

    str[count] = '\0';
    return str;
}

TValue make_string(char* str) {
    size_t stlen = strlen(str);
    char* nStr = malloc(stlen + 1);
    strcpy(nStr, str);

    TStr* nTStr = malloc(sizeof(TStr));
    nTStr->len = stlen;
    nTStr->str = nStr;
    uint64_t ptr = (uint64_t)(nTStr);
    uint64_t value = STRING | (PAYLOAD_MASK & ptr);
    return (TValue){.nan = value};
}