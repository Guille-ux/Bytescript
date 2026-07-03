#ifndef NAMESPACES_H
#define NAMESPACES_H

#include "Values/bytev.h"

typedef struct {
    CDATA_HEADER;
    TValue value;
    TTable* nmspace;
} NamespaceEntry;

extern TCDataType NamespaceEntry_Type, *NamespaceEntryType;

TValue new_namespace_entry(TValue value, TTable* map);
TValue set_namespace_value(TValue namespace, TValue value);
TValue get_namespace_value(TValue namespace);
TValue get_namespace_map(TValue namespace);
NamespaceEntry* as_namespace_entry(TValue value);

#endif