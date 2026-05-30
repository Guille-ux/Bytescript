#include "utils/utils.h"


TValue shift_tvalue_array(TValue arr[], size_t size, int start) {
    for (int i = start; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}

uint8_t shift_uint8_array(uint8_t arr[], size_t size, int start) {
    for (int i = start; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
}