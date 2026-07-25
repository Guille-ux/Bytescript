#ifndef TTIME_H
#define TTIME_H

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

TValue time_time(TValue* argv, size_t argc);
TValue clock_time(TValue* argv, size_t argc);
TValue sleep_time(TValue* argv, size_t argc);

#endif