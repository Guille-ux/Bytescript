#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* name;
    char* details;
    size_t pst;
    size_t pen;
} Error;

typedef struct {
    uint8_t sucess;
    Error* err;
} Response;



Error* new_error(char* name, char* details, uint8_t pst, uint8_t pen);
void call_error(Error* err, char* filetext);


#define TYPE_ERROR(details)                 (call_error(new_error("TypeError", (details), 0, 0), NULL))
#define INVALID_ARGS_ERROR(details)         (call_error(new_error("InvalidArgsError", (details), 0, 0), NULL))
#define FAILED_ALLOCATION_ERROR(details)    (call_error(new_error("FailedAllocationError", (details), 0, 0), NULL))
#define INDEX_OUT_OF_SCOPE_ERROR(details)   (call_error(new_error("IndexOutOfScopeError", (details), 0, 0), NULL))
#define SYNTAX_ERROR(details)               (call_error(new_error("InvalidSyntaxError", (details), 0, 0), NULL))
#define STACK_OVERFLOW(details)             (call_error(new_error("StackOverflowError", (details), 0, 0), NULL))
#define INCORRECT_LVALUE(details)           (call_error(new_error("IncorrectLvalue", (details), 0, 0), NULL))
#define UNKNOWN_IDENTIFIER(details)         (call_error(new_error("UnknownIdentifier", (details), 0, 0), NULL))


#endif