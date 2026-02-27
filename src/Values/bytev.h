#ifndef TRASH_VALUE
#define TRASH_VALUE

#include <stdint.h>


#define LJ_ALIGN(x) __attribute__((aligned(x)))


// TYPEDEFS

typedef uint64_t TBool;
typedef double TNumber;

typedef LJ_ALIGN(8) union TrashValue {
    uint64_t nan;
    TNumber  num;
} TValue;

typedef struct Func {
    const uint8_t* bytecode;
    const TValue* constPool;
    TValue* const * upvalues;
} TFunc;

typedef struct Str {
    size_t len;
    char* str;
} TStr;

typedef struct Table {
    size_t len;
    size_t capacity;
    TValue* array;
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

#define TYPE_MASK       (((0x01ULL << TYPE_AMOUNT) - 1) << TYPE_SHIFT)
#define TYPE_STRING     (0x01ULL << TYPE_SHIFT)
#define TYPE_FUNC       (0x02ULL << TYPE_SHIFT)
#define TYPE_TABLE      (0x03ULL << TYPE_SHIFT)

#define PAYLOAD_MASK    (~(QNaN | TYPE_MASK | 0x01ULL << 63))

#define NIL             (QNaN)
#define STRING          (TYPE_STRING | QNaN)
#define FUNC            (TYPE_FUNC | QNaN)
#define TABLE           (TYPE_TABLE | QNaN)

#define IS_NUM(v)       ((AS_TVALUE(v) & NaN) != NaN)
#define IS_STRING(v)    ((AS_TVALUE(v) & ~PAYLOAD_MASK) == STRING)
#define IS_FUNC(v)      ((AS_TVALUE(v) & ~PAYLOAD_MASK) == FUNC)
#define IS_TABLE(v)     ((AS_TVALUE(v) & ~PAYLOAD_MASK) == TABLE)

#define NAN_AS_TVALUE(v)((TValue){ .nan = (v) })
#define NUM_AS_TVALUE(v)((TValue){ .num = (v) })
#define AS_TVALUE(v)    (v.nan)
#define AS_NUM(v)       (v.num)

#define PAYLOAD(v)      (AS_TVALUE(v) & PAYLOAD_MASK)
#define AS_STRING(v)    ( ( TStr* )( PAYLOAD(v) ) )


#endif