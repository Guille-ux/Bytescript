#ifndef BYTE_VALUE_H
#define BYTE_VALUE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LJ_ALIGN(x) __attribute__((aligned(x)))


// TYPEDEFS


typedef uint64_t TBool;
typedef double TNumber;

typedef LJ_ALIGN(8) union BValue {
    uint64_t nan;
    TNumber  num;
} TValue;


// MAIN TYPES


typedef struct Func {
    const uint8_t* bytecode;
    const TValue* constPool;
    TValue* const * upvalues;
} TFunc;

typedef struct Str {
    size_t len;
    char str[];
} TStr;


// TABLES


typedef struct {
    size_t len;
    size_t capacity;
    TValue* array;
} Array;

typedef struct {
    TValue key;
    TValue value;
} Pair;

typedef struct {
    size_t len;
    size_t capacity;
    size_t tombstones;
    Pair* arr;
} PairArray;

typedef struct {
    Pair pair;
    PairArray sub;
} Bucket;

typedef struct {
    size_t len;
    size_t capacity;
    Bucket* array;
} HashTable;

typedef struct Table {
    Array array;
    HashTable hash_table;
} TTable;

typedef TValue (*CFunc)(TValue* args, uint8_t nArgs);


// NAN BOXING


typedef TValue* TStack;
#define TVALUESIZE sizeof(TValue)

#define EXP_SHIFT 52
#define Q_SHIFT 51

#define TYPE_AMOUNT 3
#define TYPE_SHIFT (Q_SHIFT - TYPE_AMOUNT)



#define NaN             (0x7ffULL << EXP_SHIFT)
#define QNaN_shift      (0x01ULL << 51)
#define QNaN            (NaN | QNaN_shift)

#define NAN_AS_TVALUE(v)((TValue){ .nan = (v) })
#define NUM_AS_TVALUE(v)((TValue){ .num = (v) })

#define AS_TVALUE(v)    (v.nan)
#define AS_NUM(v)       (v.num)

#define TYPE_MASK       (((0x01ULL << TYPE_AMOUNT) - 1) << TYPE_SHIFT)
#define TYPE_STRING     (0x01ULL << TYPE_SHIFT)
#define TYPE_FUNC       (0x02ULL << TYPE_SHIFT)
#define TYPE_TABLE      (0x03ULL << TYPE_SHIFT)

#define PAYLOAD_MASK    (~(QNaN | TYPE_MASK | 0x01ULL << 63))

#define NIL             (NAN_AS_TVALUE(QNaN))

#define STRING          (TYPE_STRING | QNaN)
#define FUNC            (TYPE_FUNC | QNaN)
#define TABLE           (TYPE_TABLE | QNaN)

#define IS_NIL(v)       (AS_TVALUE(v) == AS_TVALUE(NIL))
#define IS_NUM(v)       ((AS_TVALUE(v) & NaN) != NaN)
#define IS_STRING(v)    ((AS_TVALUE(v) & ~PAYLOAD_MASK) == STRING)
#define IS_FUNC(v)      ((AS_TVALUE(v) & ~PAYLOAD_MASK) == FUNC)
#define IS_TABLE(v)     ((AS_TVALUE(v) & ~PAYLOAD_MASK) == TABLE)

#define PAYLOAD(v)      (AS_TVALUE(v) & PAYLOAD_MASK)
#define TYPE(v)         (AS_TVALUE(v) & (TYPE_MASK | QNaN))

#define AS_STRING(v)    ( ( TStr* )( PAYLOAD(v) ) )
#define AS_FUNC(v)      ( ( TFunc* )( PAYLOAD(v) ) )
#define AS_TABLE(v)     ( ( TTable* )( PAYLOAD(v) ) )

#define STR_TO_TVALUE(v)( NAN_AS_TVALUE(( (uint64_t)(v) ) | STRING) )
#define FNC_TO_TVALUE(v)( NAN_AS_TVALUE(( (uint64_t)(v) ) | FUNC) )
#define TBL_TO_TVALUE(v)( NAN_AS_TVALUE(( (uint64_t)(v) ) | TABLE) )

#define TRUTHY(v)       ( AS_NUM(v) && !IS_NIL(v) )
#define FALSY(v)        ( !AS_NUM(v) || IS_NIL(v) )

#define BSIZE(v)        (TVALUESIZE * v)


static inline TValue make_substring(char* string, size_t len) {
    TStr* str = malloc(sizeof(TStr) + len + 1);
    strncpy(str->str, string, len);

    str->str[len] = '\0';
    str->len = len;

    return STR_TO_TVALUE(str);
}


#endif