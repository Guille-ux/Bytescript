#include "Globals/globals.h"
#include "Globals/string.h"
#include "Globals/number.h"
#include "Globals/function.h"
#include "Globals/IO/IO.h"


TTable* globals_namespace;
TValue globals_map;
void initialize_globals() {
    globals_namespace = AS_TABLE(new_table(NULL, 0));
    globals_map = TBL_TO_TVALUE(globals_namespace);

    printf("function\n");
    initialize_function_class();
    printf("number\n");
    initialize_number_class();
    printf("string\n");
    initialize_string_class();
    printf("table\n");
    initialize_table_class();
    printf("error\n");
    initialize_error_class();
    printf("io\n");
    initialize_io_class();
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
