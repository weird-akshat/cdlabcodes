#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fptr1, *fptr2, *fptr3;
    char filename[100];
    char c;
    int mode = 0;
    int f1end = 0;
    int f2end = 0;
    int f1active = 0;
    int f2active = 0;

    printf("Enter the first file: ");
    scanf("%s", filename);
    fptr1 = fopen(filename, "r");
    if (fptr1 == NULL) {
        printf("Can't open file %s\n", filename);
        return -1;
    }

    printf("Enter the second file: ");
    scanf("%s", filename);
    fptr2 = fopen(filename, "r");
    if (fptr2 == NULL) {
        printf("Can't open file %s\n", filename);
        fclose(fptr1);
        return -1;
    }

    printf("Enter the file to copy to: ");
    scanf("%s", filename);
    fptr3 = fopen(filename, "w");
    if (fptr3 == NULL) {
        printf("Can't open file %s\n", filename);
        fclose(fptr1);
        fclose(fptr2);
        return -1;
    }

    while (!f1end || !f2end) {
        if (!f1end && (mode == 0 || f2end)) {
            c = fgetc(fptr1);
            if (c != EOF) {
                fputc(c, fptr3);
                if (c == '\n') {
                    mode = 1;
                    f1active = 0;
                } else {
                    f1active = 1;
                }
            } else {
                if (f1active) {
                    fputc('\n', fptr3);
                    f1active = 0;
                }
                f1end = 1;
                mode = 1;
            }
        }

        if (!f2end && (mode == 1 || f1end)) {
            c = fgetc(fptr2);
            if (c != EOF) {
                fputc(c, fptr3);
                if (c == '\n') {
                    mode = 0;
                    f2active = 0;
                } else {
                    f2active = 1;
                }
            } else {
                if (f2active) {
                    fputc('\n', fptr3);
                    f2active = 0;
                }
                f2end = 1;
                mode = 0;
            }
        }
    }

    printf("Completed\n");

    fclose(fptr1);
    fclose(fptr2);
    fclose(fptr3);

    return 0;
}