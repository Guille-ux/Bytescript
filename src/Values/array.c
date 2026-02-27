#include "Globals/array.h"




TValue free_table(TValue* args, TValue nArgs) {
    TValue returnv[1];
    returnv[0] = NUM_AS_TVALUE(((double)(1)));

    for (int i = 1; i <= AS_NUM(nArgs); i++) {
        TValue value = *(args + i);
        switch (AS_TVALUE(value) & (QNaN | TYPE_MASK)) {
            case STRING: {
            uint64_t p = (AS_TVALUE(value) & PAYLOAD_MASK);
            TStr* ptr = (TStr*)(p);
            free(ptr->str);
            free(ptr);
            break;
        }

        case FUNC: {
            uint64_t p = (AS_TVALUE(value) & PAYLOAD_MASK);
            TFunc* ptr = (TFunc*)(p);
            free(ptr);
            break;
        }

        case TABLE: {
            uint64_t p = (AS_TVALUE(value) & PAYLOAD_MASK);
            TArray* ptr = (TArray*)(p);
            free(ptr->array);
            free(ptr);
            break;
        }

        default:
            break;
        }
    }

    return to_return_value(returnv, (size_t)(1));
}