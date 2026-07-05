#include "Globals/globals.h"
#include "Globals/string.h"
#include "Globals/number.h"
#include "Globals/function.h"
#include "Globals/IO/IO.h"
#include "Globals/IO/File.h"
#include "utils/disassembler.h"


TValue globals;
TTable* globals_namespace;
TValue globals_map;
void initialize_globals() {
    globals = new_table(NULL, 0);
    globals_namespace = AS_TABLE(new_table(NULL, 0));
    globals_map = TBL_TO_TVALUE(globals_namespace);

    initialize_function_class();
    initialize_number_class();
    initialize_string_class();
    initialize_table_class();
    initialize_error_class();
    initialize_io_class();
}

int add_global(TValue value, TValue name) {
    TTable* glob_table = AS_TABLE(globals);
    int idx = glob_table->array.len;

    add_value_to_hashtable(globals, name, NUM_AS_TVALUE(idx));
    append_to_table(globals, value);

    return idx;
}

TValue get_global(TValue name) {
    return get_from_table(globals, get_value_from_hashtable(globals, name));
}


TValue print(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue v = argv[i];

        switch (TYPE(v)) {
            case STRING : {
                TStr* ptr = AS_STRING(v);
                printf("%s", ptr->str);
                break;
            }

            case TABLE : {
                TTable* ptr = AS_TABLE(v);
                printf("table <%p>", ptr);
                break;
            }

            case FUNC : {
                TFunc* ptr = AS_FUNC(v);
                printf("function <%p>", ptr);
                break;
            }

            default : {
                double num = AS_NUM(v);
                printf("%f", num);
                break;
            }
        }

        printf("\t");
    }

    printf("\n");
    return NIL;
}

TValue tfree(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue v = argv[i];

        switch (TYPE(v)) {
            case STRING : {
                TStr* ptr = AS_STRING(v);
                free(ptr);
                break;
            }

            case TABLE : {
                free_table(&v, 1);
                break;
            }

            case FUNC : {
                TFunc* ptr = AS_FUNC(v);
                if (ptr->isCfunc) {
                    free((void*)ptr->bytescriptFunction.bytecode);
                    free((void*)ptr->bytescriptFunction.upvalues);

                } else free(ptr->cfunc);

                free(ptr);
                break;
            }
        }
    }

    return NIL;
}

TValue disassemble(TValue* argv, size_t argc) {
    MIN_ARGC(1);
    if (TYPE(argv[0]) != FUNC) call_error(new_type_error, "function <disassemble> take type <Function> as first argument");
    if (argc > 1 && TYPE(argv[1]) != STRING) call_error(new_type_error, "function <disassemble> take type <String> as second argument");

    TValue open_args[2] = { argv[1], make_fstring("w") };

    TValue func = argv[0];
    TValue file = argc > 1 ? open_IO(open_args, 2) : IO_stdout;

    TValue disassembled = full_disassemble(func);
    write_file((TValue[2]){ file, disassembled }, 2);
}