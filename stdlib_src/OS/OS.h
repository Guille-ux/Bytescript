#ifndef TOS_H
#define TOS_H

#include "bytescript.h"

struct StrLinkedList;
struct StrLinkedList {
    char* str;
    size_t len;
    struct StrLinkedList* next;
};

struct MiddleStr {
    char* str;
    size_t len;
};

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

TValue env_os(TValue* argv, size_t argc);
TValue cwd_os(TValue* argv, size_t argc);
TValue chdir_os(TValue* argv, size_t argc);
TValue absolute_os(TValue* argv, size_t argc);
TValue normalize_os(TValue* argv, size_t argc);
TValue filename_os(TValue* argv, size_t argc);
TValue stem_os(TValue* argv, size_t argc);
TValue ext_os(TValue* argv, size_t argc);
TValue parent_os(TValue* argv, size_t argc);
TValue join_os(TValue* argv, size_t argc);
TValue run_os(TValue* argv, size_t argc);
TValue exit_os(TValue* argv, size_t argc);

static size_t linked_str_len(struct StrLinkedList head);
static void linked_str_to_array(struct StrLinkedList head, struct MiddleStr arr[], size_t argc);

#endif