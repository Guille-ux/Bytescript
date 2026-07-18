#ifndef ERROR_H
#define ERROR_h

#include "Values/bytev.h"
#include "Values/oop.h"
#include "Globals/globals.h"

extern TValue Error_class;
extern TValue TypeError;
extern TValue IndexError;
extern TValue SyntaxError;
extern TValue InvalidArgsError;
extern TValue StackOverflowError;
extern TValue IncorrectLvalueError;
extern TValue FailedAllocationError;
extern TValue InvalidIdentifierError;
extern TValue UnknownIdentifierError;
extern TValue UnidentifiedCharacterError;

void initialize_error_class();

TValue new_error(TValue* argv, size_t argc);
TValue new_type_error(TValue* argv, size_t argc);
TValue new_index_error(TValue* argv, size_t argc);
TValue new_syntax_error(TValue* argv, size_t argc);
TValue new_invalid_args_error(TValue* argv, size_t argc);
TValue new_stack_overflow_error(TValue* argv, size_t argc);
TValue new_incorrect_lvalue_error(TValue* argv, size_t argc);
TValue new_failed_allocation_error(TValue* argv, size_t argc);
TValue new_invalid_identifier_error(TValue* argv, size_t argc);
TValue new_unknown_identifier_error(TValue* argv, size_t argc);
TValue new_unidentified_character_error(TValue* argv, size_t argc);

void call_error(CFunc err_func, char details[]);

#endif