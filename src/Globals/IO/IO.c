#include "Globals/IO/IO.h"
#include "Globals/IO/File.h"
#include "Globals/string.h"
#include "bytescript.h"


TValue IO_class;
TValue IO_obj_class;
void initialize_io_class() {
    TValue IO_fields[] = {
        new_cfunc(open_IO)
    };

    TValue IO_obj_fields[] = {

    };

    easy_map("src/Globals/maps/IO_map.bs");
    IO_class = new_class(NULL, IO_fields, 0, sizeof(IO_fields) / sizeof(IO_fields[0]));
    IO_obj_class = new_class(NULL, NULL, 0, sizeof(IO_obj_fields) / sizeof(TValue));
    initialize_file_class();
}

TValue open_IO(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "IO.open")

    TValue filename = argv[1];
    TValue openmode = argc > 2 ? argv[3] : make_fstring("r");

    char* filename_str = AS_STRING(filename)->str;
    char* openmode_str = AS_STRING(openmode)->str;

    FILE* pFile = fopen(filename_str, openmode_str);
    if (!pFile) {
        TValue error_message = make_fstring("Could not open file '%s'");
        error_message = format_string((TValue[2]){ error_message, filename }, 2);

        call_error(new_failed_allocation_error, AS_STRING(error_message)->str);
    }

    TValue file_data = new_filedata(pFile);

    TValue file = new_object(File_class, NULL, 1);
    TObject* tfile = AS_OBJECT(file);
    tfile->fields[0] = file_data;

    return file;
}
