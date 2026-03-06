#include "Globals/builtins.h"




TValue print(TValue* argv, size_t argc) {
    for (int i = 0; i < argc; i++) {
        TValue v = argv[i];

        switch (TYPE(v)) {
            case STRING : {
                TStr* ptr = AS_STRING(v);
                printf("%s", ptr->str);
            }

            case TABLE : {
                TTable* ptr = AS_TABLE(v);
                printf("table <%p>", ptr);
            }

            case FUNC : {
                TFunc* ptr = AS_FUNC(v);
                printf("function <%p>", ptr);
            }

            default : {
                double num = AS_NUM(v);
                printf("%f", num);
            }
        }

        printf("\t");
    }

    printf("\n");
    return NIL;
}