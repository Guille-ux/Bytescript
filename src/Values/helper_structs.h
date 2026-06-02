#ifndef HELPER_STRUCT_H
#define HELPER_STRUCT_H

#include "stdint.h"
#include "Values/bytev.h"

struct uint8LinkedList {
    uint8_t* ptr;
    struct uint8LinkedList* next;
};

struct TValueLinkedList {
    TValue value;
    struct TValueLinkedList* next;
};

#endif