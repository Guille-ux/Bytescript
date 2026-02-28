#include "Globals/table.h"



TValue new_table(TValue* argv, uint8_t argc) {
        uint8_t cap = argc;
        if (cap == 0) cap = 4;

        TValue* newarray = malloc(sizeof(TValue) * argc);
        TTable* newtab = malloc(sizeof(TTable));
        if (newarray == NULL || newtab == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.new>");

        newtab->array = newarray;      
        newtab->capacity = cap;         
        newtab->len = argc;

        for (int i = 1; i <= argc; i++) {
            newarray[i] = argv[i];
        }

        size_t returnc = 1;
        TValue returnv[returnc];
        returnv[0] = TBL_TO_TVALUE(newtab);
        return to_return_value(returnv, returnc);
}

TValue free_table(TValue* argv, uint8_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) TYPE_ERROR("function <table.free> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        free(ptr->array);
        free(ptr);
    }

    return NIL;
}

TValue pop_table(TValue* argv, uint8_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.pop> take type <Table> as first argument");

    size_t returnc = argc - 1 ? argc - 1 : argc;
    TTable* ptr = AS_TABLE(argv[0]);
    TValue returnv[returnc];

    if (argc == 1) {
        returnv[0] = ptr->array[ptr->len - 1];
        ptr->array[ptr->len - 1] = NIL;
        ptr->len -= 1;
    }

    for (int i = 1; i < argc; i++) {
        int idx = AS_NUM(argv[i]);
        if ( OUT_OF_SCOPE(ptr, idx) ) INDEX_OUT_OF_SCOPE_ERROR("Index out of scope in function <table.pop>");

        returnv[i - 2] = ptr->array[idx];
        ptr->array[idx] = NIL;
        ptr->len -= 1;
    }

    shift_table(ptr, returnc);
    potential_shrink(ptr);
    return to_return_value(returnv, returnc);
}

TValue shrink_table(TValue* argv, uint8_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.shrink> take type <Table> as first argument");
    TTable* ptr = AS_TABLE(argv[0]);
    
    ptr->capacity = ptr->capacity / 2;
    TValue* nptr = realloc(ptr->array, sizeof(TValue) * ptr->capacity);
    ptr->array = nptr;

    if (ptr->array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.expand>");
}

TValue expand_table(TValue* argv, uint8_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.shrink> take type <Table> as first argument");
    TTable* ptr = AS_TABLE(argv[0]);

    ptr->capacity *= 2;
    TValue* nptr = realloc(ptr->array, sizeof(TValue) * ptr->capacity);
    ptr->array = nptr;

    if (ptr->array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.expand>");
}

TValue len_table(TValue* argv, uint8_t argc) {
    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) TYPE_ERROR("function <table.len> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        returnv[i] = NUM_AS_TVALUE(ptr->len);
    }

    return to_return_value(returnv, returnc);
}

TValue reserve_table(TValue* argv, uint8_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.reserve> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    size_t n_size = AS_NUM(argv[1]);

    if (ptr->capacity < n_size) {
        TValue* nptr = realloc(ptr->array, BSIZE(n_size));
        ptr->array = nptr;
        ptr->capacity = n_size;

        if (ptr->array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.reserve>");
    }
}