#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fptr1;
    char filename[100];
    int c; 
    int lines = 0;
    int characters = 0;

    printf("Enter the file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    fptr1 = fopen(filename, "r");

    if (fptr1 == NULL) {
        printf("Can't open file %s\n", filename);
        return -1;
    }

    c = fgetc(fptr1);
    if (c != EOF) {
        lines = 1;
    }

    while (c != EOF) {
        if (c == '\n') {
            lines++;
        } else {
            characters++;
        }
        c = fgetc(fptr1);
    }

    printf("Number of lines: %d\nNumber of characters: %d\n", lines, characters);
    fclose(fptr1);

    return 0;
}