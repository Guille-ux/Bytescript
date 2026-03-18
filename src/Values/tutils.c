#include "Values/tutils.h"
#include "Error/error.h"
#include "Lexer/lexer.h"


TValue to_return_value(TValue returnv[], size_t returnc) {
    if (returnc == 1) return *returnv;
    if (returnc == 0) return NIL;
    return new_table(returnv, returnc);
}

TValue potential_shrink(TTable* table) {
    if (table->array.len <= table->array.capacity / 2) {
        TValue tv_table = TBL_TO_TVALUE(table);
        shrink_table(&tv_table, 1);
    }
}

TValue make_string(char* str) {
    return make_substring(str, strlen(str));
}

TTable* shift_table(TTable* table, size_t dead_values) {
    for (int i = 0; i < table->array.len + dead_values; i++) {
        if (IS_NIL(table->array.array[i])) {
            table->array.array[i] = table->array.array[i + 1];
            table->array.array[i + 1] = NIL;
        }
    }

    return table;
}

TTable* shift_table_right(TTable* table, size_t idx) {
    for (int i = table->array.len; i > idx; i--) {
        table->array.array[i] = table->array.array[i - 1];
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

void print_tokens(Token* tokens, size_t tok_len) {
    printf("\n[\n");

    for (int i = 0; i < tok_len; i++) {
        printf("\tTT : %3d; Value : ", tokens[i].tt);
        print(&tokens[i].value, 1);
    }

    printf("]\n");
}