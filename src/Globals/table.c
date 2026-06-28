#include <stdbool.h>
#include <inttypes.h>

#include "Values/oop.h"
#include "Globals/table.h"
#include "Globals/string.h"
#include "Globals/globals.h"


TValue initialize_table_class() {
    TValue Table_methods[][2] = {
        NEW_METHOD(new, table),
        NEW_METHOD(free, table),
        NEW_METHOD(pop, table),
        NEW_METHOD(shrink, table),
        NEW_METHOD(expand, table),
        NEW_METHOD(len, table),
        NEW_METHOD(reserve, table),
        NEW_METHOD(print, table),
        NEW_METHOD(slice, table),
        NEW_METHOD(fill, table),
        NEW_METHOD(copy, table),
        NEW_METHOD(insert, table),
        NEW_METHOD(hash_value, table),
        NEW_METHOD(append_hash, table),
        NEW_METHOD(get, table),
        NEW_METHOD(clean, table),
        NEW_METHOD(reset, table),
        NEW_METHOD(hashed_keys, table),
        NEW_METHOD(hashed_values, table),
        NEW_METHOD(buckets, table)
    };

    size_t fields_size = sizeof(Table_methods) / sizeof(Table_methods[0]);
    TValue Table_map = new_table(NULL, 0);
    TValue Table_fields[fields_size];

    append_methods_to_map(Table_map, Table_methods, Table_fields, fields_size);

    TValue Table_class = new_class(NULL, Table_fields, 0, fields_size);
    TValue Table_namespace = new_namespace_entry(Table_class, AS_TABLE(Table_map));

    add_value_to_hashtable(TBL_TO_TVALUE(globals_namespace), make_fstring("Table"), Table_namespace);
    return Table_class;
}


TValue new_table(TValue* argv, size_t argc) {
    size_t arr_cap = argc;
    size_t hash_cap = 16;

    if (arr_cap == 0) arr_cap = 4;

    TValue* newarray = malloc(BSIZE(arr_cap));
    TTable* newtab = malloc(sizeof(TTable));
    Bucket* newhasharray = malloc(sizeof(Bucket) * hash_cap);
    if (newarray == NULL || newtab == NULL || newhasharray == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.new>");

    newtab->array.array = newarray;      
    newtab->array.capacity = arr_cap;         
    newtab->array.len = argc;

    newtab->hash_table.array = newhasharray;
    newtab->hash_table.capacity = hash_cap;
    newtab->hash_table.len = 0;

    for (int i = 0; i < argc; i++) newarray[i] = argv[i];
    reset_buckets(newhasharray, hash_cap);

    TValue tab_tvalue = TBL_TO_TVALUE(newtab);
    return to_return_value(&tab_tvalue, 1);
}

TValue free_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.appendKey> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);

    for (int i = 0; i < ptr->hash_table.capacity; i++) {
        Pair* sub = ptr->hash_table.array[i].sub.arr;
        if (sub != NULL) free(sub);
    }

    free(ptr->array.array);
    free(ptr->hash_table.array);
    free(ptr);

    return NIL;
}

TValue pop_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.pop> take type <Table> as first argument");

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
        if ( OUT_OF_SCOPE(ptr, idx) ) call_error(new_index_error, "Index out of scope in function <Table.pop>");

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
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.shrink> take type <Table> as first argument");
    TTable* ptr = AS_TABLE(argv[0]);
    
    ptr->array.capacity = ptr->array.capacity / 2;
    TValue* nptr = realloc(ptr->array.array, BSIZE(ptr->array.capacity));
    ptr->array.array = nptr;

    if (ptr->array.array == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.expand>");
}

TValue expand_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.expand> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    ptr->array.capacity *= 2;

    TValue* nptr = realloc(ptr->array.array, BSIZE(ptr->array.capacity));
    ptr->array.array = nptr;

    if (ptr->array.array == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.expand>");
    return NIL;
}

TValue len_table(TValue* argv, size_t argc) {
    size_t returnc = argc;
    TValue returnv[returnc];

    for (int i = 0; i < argc; i++) {
        TValue value = argv[i];
        if (!IS_TABLE(value)) call_error(new_type_error, "function <Table.len> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        returnv[i] = NUM_AS_TVALUE(ptr->array.len);
    }

    return to_return_value(returnv, returnc);
}

TValue reserve_table(TValue* argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.reserve> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    size_t n_size = AS_NUM(argv[1]);

    if (ptr->array.capacity < n_size) {
        TValue* nptr = realloc(ptr->array.array, BSIZE(n_size));
        ptr->array.array = nptr;
        ptr->array.capacity = n_size;

        if (ptr->array.array == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.reserve>");
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
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.pop> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    uint64_t start  = argc > 1 ? AS_NUM(argv[1]) : 0;
    uint64_t end    = argc > 2 ? AS_NUM(argv[2]) : 0;

    if (OUT_OF_SCOPE(ptr, start) || OUT_OF_SCOPE(ptr, end)) call_error(new_index_error, "Index out of scope in function <Table.slice>");
    return new_table(ptr->array.array + start, end - start + 1);
}

TValue fill_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.fill> take type <Table> as first argument");

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
        if (!IS_TABLE(value)) call_error(new_type_error, "function <Table.copy> take type <Table> as arguments");

        TTable* ptr = AS_TABLE(value);
        returnv[i] = new_table(ptr->array.array, ptr->array.len);
    }

    return to_return_value(returnv, returnc);
}

TValue insert_table(TValue* argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.insert> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue value = argv[1];
    size_t idx = argc > 2 ? AS_NUM(argv[2]) : ptr->array.len;

    if (ptr->array.len >= ptr->array.capacity) expand_table(argv, 1);
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

TValue append_hash_table(TValue* argv, size_t argc) {
    if (argc < 3) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.appendKey> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue key = argv[1];
    TValue value = argv[2];

    uint64_t hashed = hash_single_value(key);
    uint64_t index = hashed % ptr->hash_table.capacity;
    Bucket* buck = ptr->hash_table.array + index;

    if (EMPTY_PAIR(buck->pair) || compare_tvalues(buck->pair.key, key)) {
        buck->pair.key = key;
        buck->pair.value = value;

    } else probe_bucket(buck, hashed, key, value, ptr->hash_table);

    ptr->hash_table.len++;
    if (IS_NIL(value)) ptr->hash_table.len--;

    potential_hash_resize(ptr);
    return value;
}

TValue get_table(TValue* argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.get> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue key = argv[1];

    uint64_t hashed = hash_single_value(key);
    uint64_t index = hashed % ptr->hash_table.capacity;
    Bucket* buck = ptr->hash_table.array + index;

    if (compare_tvalues(buck->pair.key, key)) {
        return buck->pair.value;
    } else {
        if (buck->sub.arr == NULL) make_bucket_sub(buck);

        Pair* out[2];
        linear_probe(&buck->sub, hashed, key, out);
        return out[0] == NULL ? NIL : out[0]->value;
    }
}

TValue clean_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.cleanTable> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);

    for (int i = 0; i < ptr->hash_table.capacity; i++) {
        Bucket buck = ptr->hash_table.array[i];
        PairArray sub = buck.sub;

        for (int j = 0; j < sub.capacity; j++) {
            sub.arr[j].key = NIL;
            sub.arr[j].value = NIL;
        }

        sub.len = 0;
        sub.tombstones = 0;
        buck.pair.key = NIL;
        buck.pair.value = NIL;
    }

    ptr->hash_table.len = 0;
    ptr->array.len = 0;

    TValue tab_tvalue = TBL_TO_TVALUE(ptr);
    return to_return_value(&tab_tvalue, 1);
}

TValue reset_table(TValue* argv, size_t argc) {
    size_t arr_cap = argc - 1;
    size_t hash_cap = 16;

    if (arr_cap == 0) arr_cap = 4;

    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.resetTable> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);

    TValue* newarray = realloc(ptr->array.array, BSIZE(arr_cap));
    Bucket* newhasharray = realloc(ptr->hash_table.array, sizeof(Bucket) * hash_cap);
    if (newarray == NULL || newhasharray == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.resetTable>");

    ptr->array.array = newarray;      
    ptr->array.capacity = arr_cap;         
    ptr->array.len = argc - 1;

    ptr->hash_table.array = newhasharray;
    ptr->hash_table.capacity = hash_cap;
    ptr->hash_table.len = 0;

    for (int i = 1; i < argc; i++) newarray[i] = argv[i];
    reset_buckets(newhasharray, hash_cap);

    TValue tab_tvalue = TBL_TO_TVALUE(ptr);
    return to_return_value(&tab_tvalue, 1);
}

TValue hashed_keys_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.hashedKeys> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    return to_return_value((TValue[1]){ collect_bucket(ptr, collect_key) }, 1);
}

TValue hashed_values_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.hashedValues> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    return to_return_value((TValue[1]){ collect_bucket(ptr, collect_value) }, 1);
}

TValue buckets_table(TValue* argv, size_t argc) {
    if (argc < 1) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.appendKey> take type <Table> as first argument");

    //
}

TValue in_array_table(TValue*  argv, size_t argc) {
    if (argc < 2) return NIL;
    if (!IS_TABLE(argv[0])) call_error(new_type_error, "function <Table.inArray> take type <Table> as first argument");

    TTable* ptr = AS_TABLE(argv[0]);
    TValue value = argv[1];
    for (int i = 0; i < ptr->array.len; i++) if (EQUAL(ptr->array.array[i], value)) return TRUE_TVALUE;

    return FALSE_TVALUE;
}



static inline uint64_t hash_single_value(TValue value) {
    uint64_t v = AS_TVALUE(value);

    switch (TYPE(value)) {
        case STRING : {
            TStr* str = AS_STRING(value);
            v = rapidhash(str->str, str->len);
            break;
        }

        case TABLE : {
            TTable* tbl = AS_TABLE(value);
            v = rapidhash(&tbl->array.array, BSIZE(tbl->array.len));
            break;
        }

        default : {
            v = rapidhash(&v, sizeof(v));
            break;
        }
    }

    return v;
}

static inline void empty_pairs(Pair* pairs, size_t count) {
    for (int i = 0; i < count; i++) {
        pairs[i].key = NIL;
        pairs[i].value = NIL;
    }
}

static inline void empty_buckets(Bucket* bucks, size_t count) {
    for (int i = 0; i < count; i++) {
        empty_pairs(&bucks[i].pair, 1);
        if (bucks[i].sub.arr != NULL) empty_pairs(bucks[i].sub.arr, bucks[i].sub.capacity);
    }
}

static inline void reset_buckets(Bucket* bucks, size_t count) {
    for (int i = 0; i < count; i++) {
        bucks[i].sub.arr = NULL;
        bucks[i].sub.capacity = 0;
        bucks[i].sub.len = 0;
        bucks[i].sub.tombstones = 0;
        empty_pairs(&bucks[i].pair, 1);
    }
}


static inline void potential_tombstone(Pair* last_tombstone, Pair* found_pair, TValue key, TValue value, uint8_t* placed) {
    if (last_tombstone == NULL) {
        found_pair->key = key;
        found_pair->value = value;
    } else {
        last_tombstone->key = key;
        last_tombstone->value = value;
    }

    *placed = 1;
}

static inline void make_bucket_sub(Bucket* buck) {
    size_t pair_cap = 8;
    Pair* n_sub_array = malloc(sizeof(Pair) * pair_cap);
    empty_pairs(n_sub_array, pair_cap);

    buck->sub.arr = n_sub_array;
    buck->sub.capacity = pair_cap;
}

static inline void rehash_into(PairArray* pre, Pair* post, size_t pre_len) {
    Pair* arr = pre->arr;
    PairArray copied = *pre;
    copied.arr = post;

    for (int i = 0; i < pre_len; i++) {
        Pair pair = arr[i];

        if (!EMPTY_PAIR(pair) && !TOMBSTONE_PAIR(pair)) {
            Pair* out[2];
            uint64_t hashed = hash_single_value(pair.key);
            linear_probe(&copied, hashed, pair.key, out);

            Pair* new_pair = out[0] == NULL ? out[1] : out[0];
            new_pair = out[1] == NULL ? out[0] : out[1];

            new_pair->key = pair.key;
            new_pair->value = pair.value;
            pre->len++;
        }
    }
}

static inline void oa_rehash(Bucket* buck, size_t pre_len) {
    Pair* newhasharray = malloc(sizeof(Pair) * buck->sub.capacity);
    if (newhasharray == NULL) call_error(new_failed_allocation_error, "Fatal Error : Failed allocation in function <Table.new>");

    empty_pairs(newhasharray, buck->sub.capacity);
    buck->sub.len = 0;
    buck->sub.tombstones = 0;

    rehash_into(&buck->sub, newhasharray, pre_len);
    free(buck->sub.arr);
    buck->sub.arr = newhasharray;
}

static inline void potential_oa_resize(Bucket* buck, HashTable hash_table) {
    double load_factor = (double)buck->sub.len / buck->sub.capacity;
    double tombstone_factor = (double)buck->sub.tombstones / buck->sub.tombstones;
    bool need_rehashing = false;
    size_t pre_len = buck->sub.capacity;
    
    if (load_factor > MAX_OA_LOAD_FACTOR) {
        need_rehashing = true;
        buck->sub.capacity *= 2;

        if (buck->sub.capacity == hash_table.capacity) buck->sub.capacity *= 1.2;

    } else if (tombstone_factor > MAX_TOMBSTONE_FACTOR || (buck->sub.tombstones > buck->sub.len && buck->sub.len > 6)) need_rehashing = true;

    if (need_rehashing) oa_rehash(buck, pre_len);
}

static inline void linear_probe(PairArray* pair_array, uint64_t hashed, TValue key, Pair* out[2]) {
    Pair* last_tombstone = NULL;
    uint64_t index = hashed % pair_array->capacity;
    uint64_t i = index;
    uint8_t placed = 0;

    do {
        Pair* pair = pair_array->arr + index;

        if (EMPTY_PAIR(*pair) || compare_tvalues((*pair).key, key)) {
            out[0] = pair;
            out[1] = last_tombstone;
            return;

        } else if (TOMBSTONE_PAIR(*pair) && last_tombstone == NULL) last_tombstone = pair;

        index = ++index % pair_array->capacity;
    } while (index != i);

    out[0] = NULL;
    out[1] = last_tombstone;
}

static inline void probe_bucket(Bucket* buck, uint64_t hashed, TValue key, TValue value, HashTable hash_table) {
    if (buck->sub.arr == NULL) make_bucket_sub(buck);

    Pair* out[2];
    linear_probe(&buck->sub, hashed, key, out);

    Pair* found_pair = out[0] == NULL ? out[1] : out[0];
    found_pair = out[1] == NULL ? out[0] : out[1];

    if (compare_tvalues(out[0]->key, key) && found_pair != out[0]) { out[0]->value = NIL; buck->sub.tombstones++; }
    if (!compare_tvalues(found_pair->key, key) && TOMBSTONE_PAIR(buck->pair)) {
        buck->pair.key = key;
        buck->pair.value = value;

    } else  {
        found_pair->key = key;
        found_pair->value = value;
    }

    buck->sub.len++;
    if (IS_NIL(value)) {buck->sub.tombstones++; buck->sub.len--;}

    potential_oa_resize(buck, hash_table);
}


static inline void resize_hash_table(TTable* table) {
    HashTable* hash_table = &table->hash_table;
    Bucket* pre_arr = hash_table->array;
    size_t pre_len = hash_table->capacity;

    double load_factor = (double)hash_table->len / hash_table->capacity;

    hash_table->capacity *= 2;
    hash_table->len = 0;

    Bucket* newhasharray = malloc(sizeof(Bucket) * hash_table->capacity);
    reset_buckets(newhasharray, hash_table->capacity);
    hash_table->array = newhasharray;

    TValue args[3];
    args[0] = TBL_TO_TVALUE(table);

    for (int i = 0; i < pre_len; i++) {
        Bucket buck = pre_arr[i];
        
        if (!TOMBSTONE_PAIR(buck.pair) && !EMPTY_PAIR(buck.pair)) {
            args[1] = buck.pair.key;
            args[2] = buck.pair.value;
            append_hash_table(args, 3);
        }

        if (buck.sub.arr != NULL) {
            for (int i2 = 0; i2 < buck.sub.capacity; i2++) {
                Pair pr = buck.sub.arr[i2];
                if (!TOMBSTONE_PAIR(pr) && !EMPTY_PAIR(pr)) {
                    args[1] = pr.key;
                    args[2] = pr.value;
                    append_hash_table(args, 3);
                }
            }
        }
    }
}

static inline void potential_hash_resize(TTable* table) {
    HashTable* hash_table = &table->hash_table;
    double load_factor = (double)hash_table->len / hash_table->capacity;
    if (load_factor > MAX_LOAD_FACTOR) resize_hash_table(table);
}


static inline TValue potential_shrink(TTable* table) {
    if (table->array.len <= table->array.capacity / 2) {
        TValue tv_table = TBL_TO_TVALUE(table);
        shrink_table(&tv_table, 1);
    }
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
    for (int i = table->array.len++; i > idx; i--) {
        table->array.array[i] = table->array.array[i - 1];
    }

    return table;
}


void collect_key(TValue res, Pair* pair) { append_to_table(res, pair->key); }
void collect_value(TValue res, Pair* pair) { append_to_table(res, pair->value); }
TValue collect_bucket(TTable* ptr, BucketCollector bc) {
    TValue res = new_table(NULL, 0);
    Bucket* buckets = ptr->hash_table.array;

    for (int i = 0; i < ptr->hash_table.capacity; i++) {
        Bucket* buck = buckets + i;

        if (!IS_NIL(buck->pair.key)) {
            if (!IS_NIL(buck->pair.value)) bc(res, &buck->pair);

            if (buck->sub.arr) {
                for (int j = 0; i < buck->sub.capacity; i++) {
                    Pair* pair = buck->sub.arr + i;

                    if (!IS_NIL(pair->key) && !IS_NIL(pair->value)) bc(res, pair);
                }
            }
        }
    }

    return res;
}
