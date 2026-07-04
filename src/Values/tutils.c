#include "Values/tutils.h"
#include "Globals/tError.h"
#include "Lexer/lexer.h"


TValue to_return_value(TValue returnv[], size_t returnc) {
    if (returnc == 1) return *returnv;
    if (returnc == 0) return NIL;
    return new_table(returnv, returnc);
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
            if (!new_str) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation while reading stdout");

            str = new_str;
        }

        str[count++] = (char)c;
    }

    str[count] = '\0';
    return str;
}

void print_tokens(Token* tokens) {
    printf("\n[\n");

    do {
        printf("\tTT : %3d; Value : ", tokens->tt);
        print(&tokens->value, 1);
    } while ((tokens++)->tt != TT_EOF);

    printf("]\n");
}

void print_hash_table(TTable* tab) {
    
}

int get_linked_list_len(struct TValueLinkedList list) {
    int argc = 0;
    struct TValueLinkedList* arg = list.next;

    for (; arg; arg = arg->next) { argc++; }
    return argc;
}

void linked_list_to_array(struct TValueLinkedList list, TValue array[], size_t len) {
    TValue arg_list[len];
    struct TValueLinkedList* arg = list.next;
    for (int i = 0; i < len; (arg = arg->next ,i++)) { arg_list[i] = arg->value; }
}
