#ifndef IO_H
#define IO_H

#include "Values/bytev.h"
#include "Globals/globals.h"

extern TValue IO_class;
void initialize_io_class();

TValue open_IO(TValue* argv, size_t argc);

#endif