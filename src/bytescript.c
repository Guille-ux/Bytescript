#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <Error/error.h>
#include "Values/bytev.h"
#include "Globals/table.h"
#include "Globals/builtins.h"
#include <sys/time.h>
#include <math.h>



void get_flags();


int main(int argc, char* argv[]) {
    int doneArgc = 0;
    char* filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            exit(1);
        } else {
            filename = argv[i];
        }
    }

    if (filename == NULL) call_error(new_error("Unspecified filename", "must specify filename", 0, 0), NULL);
    FILE* pFile = fopen(filename, "r");

    if (pFile == NULL) {
        char details[snprintf(NULL, 0, "Could not find file : %s", filename) + 1];
        snprintf(details, sizeof(details), "Could not find file : %s", filename);
        call_error(new_error("Inexistant File", details, 0, 0), NULL);

        return 1;
    }

    fclose(pFile);
    return 0;
}