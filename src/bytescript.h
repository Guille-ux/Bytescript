#ifndef BYTESCRIPT_API
#define BYTESCRIPT_API

#include "bytev.h"

#define MIN_ARGC(n)                     { if (argc < n) return NIL; }
#define STRICT_MIN_ARGC(n, fname)       { if (argc < n) call_error(new_invalid_args_error, "function <" fname "> take " #n " arguments"); }
#define NUM_CHECK(n, fname)             { if (!IS_NUM(argv[n])) call_error(new_type_error, "function <" fname "> take type <Number> as " #n "th argument"); }
#define TYPE_CHECK(n, type, tnm, fname) { if (TYPE(argv[n]) != type) call_error(new_type_error, "function <" fname "> take type <" tnm "> as " #n "th argument"); }

void intialize_dependencies();
void initialize_type_specifiers();
void initialize_oop();
void initialize_globals();

TValue easy_compile(char filename[]);
TValue exec(TValue* argv, size_t argc);

TValue vm_run(TValue func);

TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count);
TValue new_object(TValue cls, CFunc constructor, size_t fields_count);
TValue import_path_finder(TValue path);

extern TValue globals;
TValue print(TValue* argv, size_t argc);
TValue tfree(TValue* argv, size_t argc);
TValue ttypeof(TValue* argv, size_t argc);
TValue disassemble(TValue* argv, size_t argc);
int add_global(TValue value, TValue name);
int make_global(TValue value, TValue name, TValue type);
int overwrite_global(TValue value, TValue name);
int make_global_function(TValue value, TValue name, TValue desc);
TValue get_global(TValue name);

TValue new_table(TValue* argv, size_t argc);
TValue free_table(TValue* argv, size_t argc);
TValue pop_table(TValue* argv, size_t argc);
TValue shrink_table(TValue* argv, size_t argc);
TValue expand_table(TValue* argv, size_t argc);
TValue len_table(TValue* argv, size_t argc);
TValue reserve_table(TValue* argv, size_t argc);
TValue print_table(TValue* argv, size_t argc);
TValue slice_table(TValue* argv, size_t argc);
TValue fill_table(TValue* argv, size_t argc);
TValue copy_table(TValue* argv, size_t argc);
TValue insert_table(TValue* argv, size_t argc);
TValue append_hash_table(TValue* argv, size_t argc);
TValue get_table(TValue* argv, size_t argc);
TValue clean_table(TValue* argv, size_t argc);
TValue reset_table(TValue* argv, size_t argc);
TValue hashed_keys_table(TValue* argv, size_t argc);
TValue hashed_values_table(TValue* argv, size_t argc);
TValue buckets_table(TValue* argv, size_t argc);
TValue in_array_table(TValue*  argv, size_t argc);
TValue hash_value_table(TValue* argv, size_t argc);

TValue compare_string(TValue* argv, size_t argc);
TValue concat_string(TValue* argv, size_t argc);
TValue format_string(TValue* argv, size_t argc);
TValue to_string_string(TValue* argv, size_t argc);
TValue make_string(char* string, size_t len);
TValue make_fstring(char* str);

extern TValue Error_class;
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

extern TValue IO_stdout;
TValue open_IO(TValue* argv, size_t argc);
TValue read_file(TValue* argv, size_t argc);
TValue write_file(TValue* argv, size_t argc);

#endif