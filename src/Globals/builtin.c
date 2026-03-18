#include "Globals/builtins.h"




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