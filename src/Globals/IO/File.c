#include "Globals/IO/File.h"

TCDataType FileData_Type = { .name = "FileData" }, *FileDataType = &FileData_Type;

TValue File_class;
TValue initialize_file_class() {
    TValue File_fields[][2] = {
        NEW_METHOD(reads, file),
        NEW_METHOD(write, file)
    };

    size_t fields_size = sizeof(File_fields) / sizeof(File_fields[0]);
    return (File_class = global_class_from_fields(File_fields, fields_size, "File"));
}


TValue reads_file(TValue* argv, size_t argc) {

}

TValue write_file(TValue* argv, size_t argc) {

}


TValue new_filedata(FILE* pfile) {
    FileData* file_data = malloc(sizeof(FileData));
    file_data->cdata_type = FileDataType;
    file_data->handle = pfile;

    return CDT_TO_TVALUE((TCData*)file_data);
}
