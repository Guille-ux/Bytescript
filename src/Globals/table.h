#ifndef TABLE_H
#define TABLE_H

#include "Values/bytev.h"
#include "Values/tutils.h"
#include "Error/error.h"
#include "Globals/builtins.h"
#include "Globals/rapidhash.h"


#define WITHIN_SCOPE(v, i)          (i < v->array.len || i >= 0)
#define OUT_OF_SCOPE(v, i)          (i >= v->array.len  || i < 0)

#define EMPTY_PAIR(pair)            (IS_NIL((pair).key))
#define TOMBSTONE_PAIR(pair)        (!IS_NIL((pair).key) && IS_NIL((pair).value))

#define MAX_LOAD_FACTOR             (0.75)
#define MAX_OA_LOAD_FACTOR          (0.7)
#define MAX_TOMBSTONE_FACTOR        (0.18)


static inline uint64_t hash_single_value(TValue value);
static inline void empty_pairs(Pair* pairs, size_t count);
static inline void empty_buckets(Bucket* bucks, size_t count);
static inline void reset_buckets(Bucket* bucks, size_t count);

static inline void potential_tombstone(Pair* last_tombstone, Pair* found_pair, TValue key, TValue value, uint8_t* placed);
static inline void make_bucket_sub(Bucket* buck);
static inline void rehash_into(PairArray* pre, Pair* post, size_t pre_len);
static inline void oa_rehash(Bucket* buck, size_t pre_len);
static inline void potential_oa_resize(Bucket* buck, HashTable hash_table);

static inline void linear_probe(PairArray* pair_array, uint64_t hashed, TValue key, Pair* out[2]);
static inline void probe_bucket(Bucket* buck, uint64_t hashed, TValue key, TValue value, HashTable hash_table);

static inline void resize_hash_table(TTable* table);
static inline void potential_hash_resize(TTable* table);

static inline TValue potential_shrink(TTable* table);
TTable* shift_table(TTable* table, size_t dead_values);
TTable* shift_table_right(TTable* table, size_t idx);


TValue new_table(TValue* argv, size_t argc);
TValue free_table(TValue* argv, size_t argc);
TValue pop_table(TValue* argv, size_t argc);
TValue shrink_table(TValue* argv, size_t argc);
TValue expand_table(TValue* argv, size_t argc);
TValue len_table(TValue* argv, size_t argc);
TValue reserve_table(TValue* argv, size_t argc);
TValue print_table(TValue* argv, size_t argc);
TValue slice_table(TValue* argv, size_t argc);
TValue fill_table(TValue* argv, size_t argc);
TValue copy_table(TValue* argv, size_t argc);
TValue insert_table(TValue* argv, size_t argc);
TValue hash_value_table(TValue* argv, size_t argc);
TValue append_key_table(TValue* argv, size_t argc);
TValue get_key_table(TValue* argv, size_t argc);


static inline void add_value_to_hashtable(TValue table, TValue key, TValue value) {
    TValue args[3] = { table, key, value };
    append_key_table(args, 3);
}

static inline TValue get_value_from_hashtable(TValue table, TValue key) {
    TValue args[2] = { table, key };
    return get_key_table(args, 2);
}


#endif