#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fp1, *fp2;
    char file_in[100], file_out[100];
    int c, next;
    int foundMain = 0;
    char word[5]; 

    printf("Input file: ");
    scanf("%s", file_in);
    fp1 = fopen(file_in, "r");

    if (fp1 == NULL) {
        printf("Error reading: %s\n", file_in);
        return -1;
    }

    printf("Output file: ");
    scanf("%s", file_out);
    fp2 = fopen(file_out, "w");

    if (fp2 == NULL) {
        printf("Error creating: %s\n", file_out);
        fclose(fp1);
        return -1;
    }

    c = getc(fp1);

    while (c != EOF) {
        if (c == '"') {
            putc(c, fp2);
            c = getc(fp1);
            while (c != '"' && c != EOF) {
                putc(c, fp2);
                if (c == '\\') {
                    c = getc(fp1);
                    putc(c, fp2);
                }
                c = getc(fp1);
            }
            if (c == '"') putc(c, fp2);
            c = getc(fp1);
            continue;
        }

        if (c == '/') {
            next = getc(fp1);
            if (next == '/') {
                while (c != '\n' && c != EOF) {
                    c = getc(fp1);
                }
                if (c == '\n') putc('\n', fp2);
                c = getc(fp1);
                continue;
            } else if (next == '*') {
                c = getc(fp1); 
                while (1) {
                    if (c == '*') {
                        next = getc(fp1);
                        if (next == '/') {
                            c = getc(fp1);
                            break;
                        }
                        fseek(fp1, -1, SEEK_CUR); 
                    }
                    if (c == EOF) break;
                    c = getc(fp1);
                }
                continue; 
            } else {
                putc(c, fp2);
                c = next;
                continue; 
            }
        }

        if (!foundMain && c == 'm') {
            long pos = ftell(fp1);
            word[0] = 'm';
            word[1] = getc(fp1);
            word[2] = getc(fp1);
            word[3] = getc(fp1);
            word[4] = '\0';

            if (strcmp(word, "main") == 0) {
                foundMain = 1;
                fprintf(fp2, "%s", word);
                c = getc(fp1);
                continue;
            } else {
                fseek(fp1, pos, SEEK_SET); 
            }
        }

        if (!foundMain && c == '#') {
            while (c != '\n' && c != EOF) {
                c = getc(fp1);
            }
            c = getc(fp1);
        } else {
            putc(c, fp2);
            c = getc(fp1);
        }
    }

    fclose(fp1);
    fclose(fp2);
    printf("Process completed\n");
    return 0;
}