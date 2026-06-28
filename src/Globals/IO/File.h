#ifndef FILE_H
#define FILE_H

#include "Values/bytev.h"
#include "Globals/globals.h"

typedef struct {
    CDATA_HEADER;
    FILE* handle;
} FileData;

extern TCDataType FileData_Type, *FileDataType;

extern TValue File_class;
TValue initialize_file_class();

TValue reads_file(TValue* argv, size_t argc);
TValue write_file(TValue* argv, size_t argc);

TValue new_filedata(FILE* pfile);

#endif