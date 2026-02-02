#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp1, *fp2;
    int char1;
    char file[100];

    printf("Input file: ");
    scanf("%s", file);
    fp1 = fopen(file, "r");
    if (fp1 == NULL) {
        printf("Error reading %s\n", file);
        exit(0);
    }

    printf("Output file: ");
    scanf("%s", file);
    fp2 = fopen(file, "w");

    char1 = getc(fp1);
    while (char1 != EOF) {
        if (char1 == ' ' || char1 == '\t') {
            putc(' ', fp2);
            while (char1 == ' ' || char1 == '\t') {
                char1 = getc(fp1);
            }
        } else {
            putc(char1, fp2);
            char1 = getc(fp1);
        }
    }

    fclose(fp1);
    fclose(fp2);
    printf("Saved to file: %s\n", file);
    return 0;
}