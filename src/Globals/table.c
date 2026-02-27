#include "Globals/table.h"




TValue free_table(TValue* argv, uint8_t argc) {
    TValue returnv[1] = {NUM_AS_TVALUE(1)};

    for (int i = 1; i <= argc; i++) {
        TValue value = *(argv + i);
        if (!IS_TABLE(value)) {TYPE_ERROR("function <table.free> take type <Table> as arguments");}

        uint64_t p = (AS_TVALUE(value) & PAYLOAD_MASK);
        TTable* ptr = (TTable*)(p);
        free(ptr->array);
        free(ptr);
    }

    return to_return_value(returnv, 1);
}

TValue pop_table(TValue* argv, uint8_t argc) {
    if (argc < 2) {INVALID_ARGS_ERROR("function <table.pop> take 2+ arguments");}
    if (!IS_TABLE(argv[1])) {TYPE_ERROR("function <table.pop> take type <Table> as first argument");}
    uint64_t p = AS_TVALUE(argv[1]) & PAYLOAD_MASK;
    TTable* ptr = (TTable*)(p);

    TValue returnv[(uint64_t)(argc - 1)];

    for (int i = 2; i <= argc; i++) {
        TValue value = argv[i];
        if (AS_NUM(value) > ptr->len - 1 || AS_NUM(value) < 0) {printf("\nINDEX OUT OF BOUNDS"); exit(1);}

        returnv[i - 2] = ptr->array[(uint64_t)(AS_NUM(value))];
        ptr->array[(uint64_t)(AS_NUM(value))] = NUM_AS_TVALUE((double)(0));
        ptr->len -= 1;
    }

    shift_table(ptr, argc - 1);

    if (ptr->len <= ptr->capacity / 2) {
        ptr->capacity = ptr->capacity / 2;
        TValue* nptr = realloc(ptr->array, sizeof(TValue) * ptr->capacity);
        if (ptr->array == NULL) {printf("FAILED ALLOCATION"); exit(1);}
        ptr->array = nptr;
    }

    return to_return_value(returnv, argc - 1);
}