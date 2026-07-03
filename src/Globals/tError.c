#include "Globals/tError.h"
#include "Globals/string.h"
#include "bytescript.h"

TValue Error_class;
TValue TypeError;
TValue IndexError;
TValue SyntaxError;
TValue InvalidArgsError;
TValue StackOverflowError;
TValue IncorrectLvalueError;
TValue FailedAllocationError;
TValue UnknownIdentifierError;
TValue UnidentifiedCharacterError;
TValue InvalidIdentifierError;


extern TValue Error_class;
void initialize_error_class() {
    TValue Error_fields[] = {
        new_cfunc(new_error)
    };

    easy_map("src/Globals/maps/error_map.bs");
    Error_class = new_class(NULL, Error_fields, 0, sizeof(Error_fields) / sizeof(Error_fields[0]));

    TypeError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_type_error) }, 1, 1);
    IndexError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_index_error) }, 1, 1);
    SyntaxError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_syntax_error) }, 1, 1);
    InvalidArgsError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_invalid_args_error) }, 1, 1);
    StackOverflowError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_stack_overflow_error) }, 1, 1);
    IncorrectLvalueError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_incorrect_lvalue_error) }, 1, 1);
    FailedAllocationError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_failed_allocation_error) }, 1, 1);
    UnknownIdentifierError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_unknown_identifier_error) }, 1, 1);
    InvalidIdentifierError = new_class(&Error_class, (TValue[1]){ new_cfunc(new_invalid_identifier_error) }, 1, 1);
}


TValue new_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "Error.new");

    TValue name = argv[0];
    TValue details = argc > 1 ? argv[1] : make_fstring("");

    printf("\n");
    printf("%s : \n\t", AS_STRING(name)->str);
    printf("%s\n", AS_STRING(details)->str);
    printf("\n");

    exit(1);
}

TValue new_type_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "TypeError.new");
    new_error((TValue[2]){ make_fstring("TypeError"), argv[0] }, 2);
}

TValue new_index_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "IndexError.new");
    new_error((TValue[2]){ make_fstring("IndexError"), argv[0] }, 2);
}

TValue new_syntax_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "SyntaxError.new");
    new_error((TValue[2]){ make_fstring("SyntaxError"), argv[0] }, 2);
}

TValue new_invalid_args_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "InvalidArgsError.new");
    new_error((TValue[2]){ make_fstring("InvalidArgsError"), argv[0] }, 2);
}

TValue new_stack_overflow_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "StackOverflowError.new");
    new_error((TValue[2]){ make_fstring("StackOverflowError"), argv[0] }, 2);
}

TValue new_incorrect_lvalue_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "IncorrectLvalueError.new");
    new_error((TValue[2]){ make_fstring("IncorrectLvalueError"), argv[0] }, 2);
}

TValue new_failed_allocation_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "FailedAllocationError.new");
    new_error((TValue[2]){ make_fstring("FailedAllocationError"), argv[0] }, 2);
}

TValue new_invalid_identifier_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "InvalidIdentifierError.new");
    new_error((TValue[2]){ make_fstring("InvalidIdentifierError"), argv[0] }, 2);
}

TValue new_unknown_identifier_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "UnknownIdentifierError.new");
    new_error((TValue[2]){ make_fstring("UnknownIdentifierError"), argv[0] }, 2);
}

TValue new_unidentified_character_error(TValue* argv, size_t argc) {
    STRICT_MIN_ARGC(1, "UnidentifiedCharacterError.new");
    new_error((TValue[2]){ make_fstring("UnidentifiedCharacterError"), argv[0] }, 2);
}

void call_error(CFunc err_func, char details[]) { err_func((TValue[1]){ make_fstring(details) }, 1); }
