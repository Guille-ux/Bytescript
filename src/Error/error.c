#include "error.h"


Error* new_error(char* name, char* details, uint8_t pst, uint8_t pen) {
    Error* err = malloc(sizeof(Error));
    err->name = name;
    err->details = details;
    err->pst = pst;
    err->pen = pen;

    return err;
}

void call_error(Error* err, char* filetext) {
    printf("\n");
    printf("%s : \n\t", err->name);
    printf("%s\n", err->details);

    if (err->pst != 0 != err->pen) {
        int linestart = err->pst;
        int lineend = err->pen;
        while (filetext[--linestart] != '\n');
        while (filetext[++lineend] != '\n');

        char buff[(--lineend) - (++linestart) + 1];
        snprintf(buff, sizeof(buff), "%s", filetext + linestart);

        printf("\n\n\t%s\n\t", buff);
        while (linestart++ != lineend) if (linestart >= err->pst && lineend <= err->pen) printf("^");;
    }

    exit(1);
}