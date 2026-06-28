#include "Globals/IO/IO.h"
#include "Globals/IO/File.h"
#include "Globals/string.h"


TValue initialize_io_class() {
    TValue IO_fields[][2] = {
        NEW_METHOD(open, IO)
    };

    size_t fields_size = sizeof(IO_fields) / sizeof(IO_fields[0]);
    return global_class_from_fields(IO_fields, fields_size, "IO");
}

TValue open_IO(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "IO.open")

    TValue self = argv[0];
    TValue filename = argv[1];
    TValue openmode = argc > 2 ? argv[3] : make_fstring("r");

    char* filename_str = AS_STRING(filename)->str;
    char* openmode_str = AS_STRING(openmode)->str;

    FILE* pFile = fopen(filename_str, openmode_str);
    TValue file_data = new_filedata(pFile);
    
    TValue file = new_object(File_class, NULL, 1);
    TObject* tfile = AS_OBJECT(file);
    tfile->fields[0] = file_data;

    return file;
}
