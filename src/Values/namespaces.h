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
NamespaceEntry* as_namespace_entry(TValue value);

#endif