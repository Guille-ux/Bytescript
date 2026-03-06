#include "Globals/table.h"


static inline uint64_t hash_single_value(TValue value) {
    uint64_t v = AS_TVALUE(value);

    switch (TYPE(value)) {
        case STRING : {
            TStr* str = AS_STRING(value);
            v = rapidhash((const uint8_t*)str->str, strlen(str->str)); 
        }

        case TABLE : {
            TTable* tbl = AS_TABLE(value);
            v = rapidhash(&tbl->array.array, BSIZE(tbl->array.len));
        }

        default : {
            v = rapidhash(&v, sizeof(v));
        }
    }

    return v;
}


TValue new_table(TValue* argv, size_t argc) {
        size_t cap = argc;
        if (cap == 0) cap = 4;

        TValue* newarray = malloc(BSIZE(cap));
        TTable* newtab = malloc(sizeof(TTable));
        BBucket* newhasharray = malloc(sizeof(BBucket) * 2);
        if (newarray == NULL || newtab == NULL || newhasharray == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.new>");

        newtab->array.array = newarray;      
        newtab->array.capacity = cap;         
        newtab->array.len = argc;
        newtab->hash_table.array = newhasharray;
        newtab->hash_table.capacity = 2;
        newtab->hash_table.len = 0;

        for (int i = 0; i < argc; i++) {
            newarray[i] = argv[i];
        }

        TValue tab_tvalue = TBL_TO_TVALUE(newtab);
        return to_return_value(&tab_tvalue, 1);
}

TValue free_table(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) TYPE_ERROR("function <table.free> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        free(ptr->array.array);
        free(ptr->hash_table.array);
        free(ptr);
    }

    return NIL;
}

TValue pop_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.pop> take type <Table> as first argument");

    size_t returnc = argc - 1 ? argc - 1 : argc;
    TTable* ptr = AS_TABLE(argv[0]);
    TValue returnv[returnc];

    if (argc == 1) {
        returnv[0] = ptr->array.array[ptr->array.len - 1];
        ptr->array.array[ptr->array.len - 1] = NIL;
        ptr->array.len -= 1;
    }

    for (int i = 1; i < argc; i++) {
        int idx = AS_NUM(argv[i]);
        if ( OUT_OF_SCOPE(ptr, idx) ) INDEX_OUT_OF_SCOPE_ERROR("Index out of scope in function <table.pop>");

        returnv[i - 2] = ptr->array.array[idx];
        ptr->array.array[idx] = NIL;
        ptr->array.len -= 1;
    }

    shift_table(ptr, returnc);
    potential_shrink(ptr);
    return to_return_value(returnv, returnc);
}

TValue shrink_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.shrink> take type <Table> as first argument");
    TTable* ptr = AS_TABLE(argv[0]);
    
    ptr->array.capacity = ptr->array.capacity / 2;
    TValue* nptr = realloc(ptr->array.array, BSIZE(ptr->array.capacity));
    ptr->array.array = nptr;

    if (ptr->array.array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.expand>");
}

TValue expand_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.expand> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    ptr->array.capacity *= 2;

    TValue* nptr = realloc(ptr->array.array, BSIZE(ptr->array.capacity));
    ptr->array.array = nptr;

    if (ptr->array.array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.expand>");
    return NIL;
}

TValue len_table(TValue* argv, size_t argc) {
    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) TYPE_ERROR("function <table.len> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        returnv[i] = NUM_AS_TVALUE(ptr->array.len);
    }

    return to_return_value(returnv, returnc);
}

TValue reserve_table(TValue* argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.reserve> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    size_t n_size = AS_NUM(argv[1]);

    if (ptr->array.capacity < n_size) {
        TValue* nptr = realloc(ptr->array.array, BSIZE(n_size));
        ptr->array.array = nptr;
        ptr->array.capacity = n_size;

        if (ptr->array.array == NULL) FAILED_ALLOCATION_ERROR("Fatal Error : Failed allocation in function <table.reserve>");
    }
}

TValue print_table(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TTable* ptr = AS_TABLE(argv[i]);

        printf("{\n");
        for (int i2 = 0; i2 < ptr->array.len; i2++) {
            printf("\t");
            print(ptr->array.array + i2, 1);
        }
        printf("}");
    }
}

TValue slice_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.pop> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    uint64_t start  = argc > 1 ? AS_NUM(argv[1]) : 0;
    uint64_t end    = argc > 2 ? AS_NUM(argv[2]) : 0;

    if (OUT_OF_SCOPE(ptr, start) || OUT_OF_SCOPE(ptr, end)) INDEX_OUT_OF_SCOPE_ERROR("Index out of scope in function <table.slice>");
    return new_table(ptr->array.array + start, end - start + 1);
}

TValue fill_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.fill> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue fillValue = argc > 1 ? argv[1] : NUM_AS_TVALUE(0);
    
    for (int i = 0; i < ptr->array.len; i++) {
        ptr->array.array[i] = fillValue;
    }

    return NIL;
}

TValue copy_table(TValue* argv, size_t argc) {
    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) TYPE_ERROR("function <table.copy> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        returnv[i] = new_table(ptr->array.array, ptr->array.len);
    }

    return to_return_value(returnv, returnc);
}

TValue insert_table(TValue* argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) TYPE_ERROR("function <table.insert> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue value = argv[1];
    size_t idx = argc > 2 ? AS_NUM(argv[2]) : ptr->array.len;

    if (ptr->array.len + 1 > ptr->array.capacity) expand_table(argv, 1);
    shift_table_right(ptr, idx);

    ptr->array.array[idx] = value;
    return value;
}

TValue hash_value_table(TValue* argv, size_t argc) {
    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        returnv[i] = NAN_AS_TVALUE(hash_single_value(value));
    }

    return to_return_value(returnv, returnc);
}