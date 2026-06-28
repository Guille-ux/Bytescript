#include "Globals/globals.h"
#include "Globals/string.h"


TTable* globals_namespace;


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

void initialize_globals() {
    globals_namespace = AS_TABLE(new_table(NULL, 0));

    initialize_string_class();
    initialize_table_class();
}

void append_methods_to_map(TValue map, TValue fields[][2], TValue fieldvalues[], size_t fields_size) {
    for (int i = 0; i < fields_size; i++) {
        fieldvalues[i] = fields[i][1];
        add_value_to_hashtable(map, fields[i][0], NUM_AS_TVALUE(i));
    }
}