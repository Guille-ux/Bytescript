#include "namespaces.h"
#include "Globals/tError.h"

TCDataType NamespaceEntry_Type = { .name = "NamespaceEntry" }, *NamespaceEntryType = &NamespaceEntry_Type;


TValue new_namespace_entry(TValue value, TTable* map) {
    NamespaceEntry* res_namespace = malloc(sizeof(NamespaceEntry));
    res_namespace->cdata_type = NamespaceEntryType;
    res_namespace->nmspace = map ? map : AS_TABLE(new_table(NULL, 0));
    res_namespace->value = value;

    return CDT_TO_TVALUE((TCData*)res_namespace);
}

TValue set_namespace_value(TValue namespace, TValue value) {
    TCData* data = AS_CDATA(namespace);
    if (*data != (TCData)NamespaceEntryType) call_error(new_type_error, "Failed to cast cdata into namespace entry");

    NamespaceEntry* entry = (NamespaceEntry*)data;
    return entry->value = value;
}

TValue get_namespace_value(TValue namespace) {
    TCData* data = AS_CDATA(namespace);
    if (*data != (TCData)NamespaceEntryType) call_error(new_type_error, "Failed to cast cdata into namespace entry");

    NamespaceEntry* entry = (NamespaceEntry*)data;
    return entry->value;
}

TValue get_namespace_map(TValue namespace) {
    TCData* data = AS_CDATA(namespace);
    if (*data != (TCData)NamespaceEntryType) call_error(new_type_error, "Failed to cast cdata into namespace entry");

    NamespaceEntry* entry = (NamespaceEntry*)data;
    return TBL_TO_TVALUE(entry->nmspace);
}

NamespaceEntry* as_namespace_entry(TValue value) {
    TCData* data = AS_CDATA(value);
    if (*data != (TCData)NamespaceEntryType) call_error(new_type_error, "Failed to cast cdata into namespace entry");
    
    return (NamespaceEntry*)data;
}