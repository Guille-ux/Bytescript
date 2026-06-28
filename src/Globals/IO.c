#include "Globals/IO.h"
#include "Globals/string.h"


TValue initialize_io_class() {
    TValue IO_fields[][2] = {
        NEW_METHOD(open, IO)
    };

    size_t fields_size = sizeof(IO_fields) / sizeof(IO_fields[0]);
    return global_class_from_fields(IO_fields, fields_size, "IO");
}

TValue open_IO(TValue* argv, size_t argc) {

}
