#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
    FILE *fp;
    char filename[100];
    char word[100];
    int c, next;
    int i;
    
    char *keywords[] = {
        "int", "float", "char", "double", "if", "else", 
        "for", "while", "return", "void", "do", "switch", 
        "case", "break", "continue", "default", "const"
    };
    int num_keywords = 17;

    printf("Input file: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error reading: %s\n", filename);
        return -1;
    }

    c = fgetc(fp);
    while (c != EOF) {
        
        if (c == '"') {
            c = fgetc(fp);
            while (c != '"' && c != EOF) {
                if (c == '\\') c = fgetc(fp); 
                c = fgetc(fp);
            }
            c = fgetc(fp); 
            continue;
        }

        if (c == '/') {
            next = fgetc(fp);
            if (next == '/') {
                while (c != '\n' && c != EOF) {
                    c = fgetc(fp);
                }
            } 
            else if (next == '*') {
                c = fgetc(fp);
                while (1) {
                    if (c == '*') {
                        next = fgetc(fp);
                        if (next == '/') {
                            c = fgetc(fp);
                            break;
                        }
                        fseek(fp, -1, SEEK_CUR);
                    }
                    if (c == EOF) break;
                    c = fgetc(fp);
                }
            } 
            else {
                fseek(fp, -1, SEEK_CUR);
            }
            c = fgetc(fp);
            continue;
        }

        if (isalpha(c) || c == '_') {
            i = 0;
            word[i++] = c;
            while (isalnum(c = fgetc(fp)) || c == '_') {
                word[i++] = c;
            }
            word[i] = '\0';
            
            fseek(fp, -1, SEEK_CUR);

            for (int k = 0; k < num_keywords; k++) {
                if (strcmp(word, keywords[k]) == 0) {
                    for (int j = 0; word[j]; j++) {
                        printf("%c", toupper(word[j]));
                    }
                    printf("\n");
                    break;
                }
            }
        }
        
        c = fgetc(fp);
    }

    fclose(fp);
    return 0;
}