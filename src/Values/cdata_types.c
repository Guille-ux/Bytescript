#include "namespaces.h"
#include "Error/error.h"

TCDataType NamespaceEntry_Type = { .name = "NamespaceEntry" }, *NamespaceEntryType = &NamespaceEntry_Type;


TValue new_namespace_entry(TValue value, TTable* map) {
    NamespaceEntry* res_namespace = malloc(sizeof(NamespaceEntry));
    res_namespace->cdata_type = NamespaceEntryType;
    res_namespace->nmspace = map;
    res_namespace->value = value;

    return CDT_TO_TVALUE((TCData*)res_namespace);
}

NamespaceEntry* as_namespace_entry(TValue value) {
    TCData* data = AS_CDATA(value);
    if (*data != (TCData)NamespaceEntryType) TYPE_ERROR("Failed to cast tvalue into namespace entry");

    return (NamespaceEntry*)data;
}