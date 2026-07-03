#include "Globals/IO/File.h"

TCDataType FileData_Type = { .name = "FileData" }, *FileDataType = &FileData_Type;

TValue File_class;
TValue File_obj_class;
void initialize_file_class() {
    TValue File_fields[] = {

    };

    TValue File_obj_fields[] = {
        new_cfunc(read_file)
    };

    File_class = new_class(NULL, NULL, 0, sizeof(File_fields) / sizeof(TValue));
    File_obj_class = new_class(NULL, File_obj_fields, 0, sizeof(File_obj_fields) / sizeof(File_obj_fields[0]));
}


TValue read_file(TValue* argv, size_t argc) {
    MIN_ARGC(1);

    TObject* obj = AS_OBJECT(argv[0]);
    TValue tfile = obj->fields[0];

    TCData* filedata = AS_CDATA(tfile);
    if (!(*filedata == FileDataType)) call_error(new_type_error, "Invalid FileData in <File> object. in function <File.read>");

    FileData* file = (FileData*)filedata;

    fseek(file->handle, 0, SEEK_END);

    long size = ftell(file->handle);
    if (size == -1) call_error(new_failed_allocation_error, "Failed allocation while reading file");
    
    rewind(file->handle);

    TStr* str = malloc(sizeof(TStr) + size + 1);
    if (!str) call_error(new_failed_allocation_error, "Failed allocation of buffer while reading file");

    size_t read = fread(str->str, 1, size, file->handle);
    str->str[read] = '\0';

    fclose(file->handle);
    return STR_TO_TVALUE(str);
}

TValue write_file(TValue* argv, size_t argc) {

}


TValue new_filedata(FILE* pfile) {
    FileData* file_data = malloc(sizeof(FileData));
    file_data->cdata_type = FileDataType;
    file_data->handle = pfile;

    return CDT_TO_TVALUE((TCData*)file_data);
}
