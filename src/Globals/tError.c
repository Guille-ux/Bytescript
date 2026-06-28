#include "Globals/tError.h"
#include "Globals/string.h"

TValue Error_class;
TValue TypeError;
TValue IndexError;
TValue SyntaxError;
TValue InvalidArgsError;
TValue StackOverflowError;
TValue IncorrectLvalueError;
TValue FailedAllocationError;
TValue UnknownIdentifierError;
extern TValue UnidentifiedCharacterError;


TValue initialize_error_class() {
    TValue Error_fields[][2] = {
        NEW_METHOD(new, error),
    };

    size_t fields_size = sizeof(Error_fields) / sizeof(Error_fields[0]);
    Error_class = global_class_from_fields(Error_fields, fields_size, "Error");

    TypeError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_type_error) }, 1, 1);
    IndexError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_index_error) }, 1, 1);
    SyntaxError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_syntax_error) }, 1, 1);
    InvalidArgsError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_invalid_args_error) }, 1, 1);
    StackOverflowError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_stack_overflow_error) }, 1, 1);
    IncorrectLvalueError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_incorrect_lvalue_error) }, 1, 1);
    FailedAllocationError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_failed_allocation_error) }, 1, 1);
    UnknownIdentifierError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_unknown_identifier_error) }, 1, 1);
    return Error_class;
}


TValue new_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "Error.new");

    TValue name = argv[1];
    TValue details = argc > 1 ? argv[2] : make_fstring("");

    printf("\n");
    printf("%s : \n\t", AS_STRING(name)->str);
    printf("%s\n", AS_STRING(details)->str);
    printf("\n");

    exit(1);
}

TValue new_type_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "TypeError.new");
    new_error((TValue[3]){ Error_class, make_fstring("TypeError"), argv[1] }, 3);
}

TValue new_index_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "IndexError.new");
    new_error((TValue[3]){ Error_class, make_fstring("IndexError"), argv[1] }, 3);
}

TValue new_syntax_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "SyntaxError.new");
    new_error((TValue[3]){ Error_class, make_fstring("SyntaxError"), argv[1] }, 3);
}

TValue new_invalid_args_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "InvalidArgsError.new");
    new_error((TValue[3]){ Error_class, make_fstring("InvalidArgsError"), argv[1] }, 3);
}

TValue new_stack_overflow_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "StackOverflowError.new");
    new_error((TValue[3]){ Error_class, make_fstring("StackOverflowError"), argv[1] }, 3);
}

TValue new_incorrect_lvalue_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "IncorrectLvalueError.new");
    new_error((TValue[3]){ Error_class, make_fstring("IncorrectLvalueError"), argv[1] }, 3);
}

TValue new_failed_allocation_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "FailedAllocationError.new");
    new_error((TValue[3]){ Error_class, make_fstring("FailedAllocationError"), argv[1] }, 3);
}

TValue new_unknown_identifier_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "UnknownIdentifierError.new");
    new_error((TValue[3]){ Error_class, make_fstring("UnknownIdentifierError"), argv[1] }, 3);
}

TValue new_unidentified_character_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(2, "UnidentifiedCharacterError.new");
    new_error((TValue[3]){ Error_class, make_fstring("UnidentifiedCharacterError"), argv[1] }, 3);
}

void call_error(CFunc err_func, char details[]) { err_func((TValue[2]){ NIL, make_fstring(details) }, 2); }
