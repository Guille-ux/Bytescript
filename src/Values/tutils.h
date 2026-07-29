#ifndef TUTILS_H
#define TUTILS_H

#include "Values/bytev.h"
#include "Globals/table.h"
#include "Globals/globals.h"
#include "Lexer/lexer.h"
#include "Values/helper_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define INT32_TO_HIHI(input)            ( (uint8_t)(((input) & 0xff0000) >> 16) )
#define INT32_TO_HI(input)              ( (uint8_t)(((input) & 0x00ff00) >> 8) )
#define INT32_TO_LO(input)              ( (uint8_t)((input) & 0x0000ff) )

static inline int32_t off24_to_int32(uint8_t hihi, uint8_t hi, uint8_t lo) {
    int32_t x = (hihi << 16) | (hi << 8) | lo;
    return (int32_t)(x << 8) >> 8;
}

static inline int16_t off16_to_int16(uint8_t hi, uint8_t lo) { return (hi << 8) | lo; }


TValue to_return_value(TValue returnv[], size_t returnc);
TValue import_path_finder(TValue path);
char* read_string();
void print_tokens(Token* tokens);
void print_hash_table(TTable* tab);
int get_linked_list_len(struct TValueLinkedList list);
void linked_list_to_array(struct TValueLinkedList list, TValue array[], size_t len);
void free_linked_list(struct TValueLinkedList list);

#endif