#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fptr1, *fptr2;
    char filename[100];
    char c;
    long fileSize;
    long i;

    printf("Enter the file to reverse: ");
    scanf("%s", filename);
    fptr1 = fopen(filename, "r");

    if (fptr1 == NULL) {
        printf("Can't open file %s\n", filename);
        return -1;
    }

    printf("Enter the file to reverse to: ");
    scanf("%s", filename);
    fptr2 = fopen(filename, "w");

    if (fptr2 == NULL) {
        printf("Can't create file %s\n", filename);
        fclose(fptr1);
        return -1;
    }

    fseek(fptr1, 0, SEEK_END);
    fileSize = ftell(fptr1);

    if (fileSize > 0) {
        fseek(fptr1, fileSize - 1, SEEK_SET);
        if (fgetc(fptr1) == '\n') {
            fileSize--;
        }
    }

    for (i = fileSize - 1; i >= 0; i--) {
        fseek(fptr1, i, SEEK_SET);
        c = fgetc(fptr1);
        fputc(c, fptr2);
    }

    printf("\nContents reversed successfully.\n");
    printf("Size of the file: %ld\n", fileSize);

    fclose(fptr1);
    fclose(fptr2);
    return 0;
}