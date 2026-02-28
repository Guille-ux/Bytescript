#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <Error/error.h>
#include "Values/bytev.h"



void get_flags();


int main(int argc, char* argv[]) {
    int doneArgc = 0;
    char* filename;

    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            exit(1);
        } else {
            filename = argv[i];
        }
    }

    
    FILE* pFile = fopen(filename, "r");
    if (pFile == NULL) {
        char details[snprintf(NULL, 0, "Could not find file : %s", filename) + 1];
        snprintf(details, sizeof(details), "Could not find file : %s", filename);
        call_error(new_error("Inexistant File", details, 0, 0), NULL);

        return 1;
    }

    
    printf("%zu", sizeof(TTable));
    fclose(pFile);
    return 0;
}