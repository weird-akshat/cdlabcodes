#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 26

struct token {
    char lexeme[100];
    int r_num, c_num;
};

struct SymbolEntry {
    char lexeme[100];
    char type[20];
    int size;
    char returnType[20];
    struct SymbolEntry *next;
};

struct SymbolEntry *hashTable[TABLE_SIZE];

int hash(char *s) {
    if (!isalpha(s[0])) return 0;
    return (tolower(s[0]) - 'a') % TABLE_SIZE;
}

struct SymbolEntry* lookup(char *s) {
    struct SymbolEntry *t = hashTable[hash(s)];
    while (t) {
        if (!strcmp(t->lexeme, s)) return t;
        t = t->next;
    }
    return NULL;
}

void insert(char *lex, char *type, int size, char *ret) {
    if (lookup(lex)) return;
    int h = hash(lex);
    struct SymbolEntry *n = malloc(sizeof(struct SymbolEntry));
    strcpy(n->lexeme, lex);
    strcpy(n->type, type);
    strcpy(n->returnType, ret);
    n->size = size;
    n->next = hashTable[h];
    hashTable[h] = n;
}

int isType(char *s) {
    return !strcmp(s,"int") || !strcmp(s,"char") ||
           !strcmp(s,"float") || !strcmp(s,"double") || !strcmp(s,"void");
}

int isKeyword(char *s) {
    char *k[] = {"int","char","float","double","void","if","else","for","while","return","main"};
    for (int i = 0; i < 11; i++)
        if (!strcmp(s,k[i])) return 1;
    return 0;
}

int typeSize(char *t) {
    if (!strcmp(t,"int") || !strcmp(t,"float")) return 4;
    if (!strcmp(t,"char")) return 1;
    if (!strcmp(t,"double")) return 8;
    return 0;
}

void stripComments(FILE *src, FILE *dest) {
    int curr, next;
    while ((curr = getc(src)) != EOF) {
        if (curr == '"') {
            putc(curr, dest);
            while ((curr = getc(src)) != '"' && curr != EOF) {
                putc(curr, dest);
                if (curr == '\\') putc(getc(src), dest);
            }
            if (curr == '"') putc(curr, dest);
            continue;
        }
        if (curr == '/') {
            next = getc(src);
            if (next == '/') {
                while ((curr = getc(src)) != '\n' && curr != EOF);
                if (curr == '\n') putc('\n', dest);
            } else if (next == '*') {
                curr = getc(src);
                while (1) {
                    if (curr == '*') {
                        next = getc(src);
                        if (next == '/') break;
                        fseek(src, -1, SEEK_CUR);
                    }
                    if (curr == '\n') putc('\n', dest);
                    if (curr == EOF) break;
                    curr = getc(src);
                }
            } else {
                putc(curr, dest);
                fseek(src, -1, SEEK_CUR);
            }
        } else {
            putc(curr, dest);
        }
    }
}

void stripDirectives(FILE *in, FILE *out) {
    int ch;
    char word[10];
    int foundMain = 0;
    int prev = ' ';

    while ((ch = getc(in)) != EOF) {
        if (ch == '"') {
            putc(ch, out);
            while ((ch = getc(in)) != '"' && ch != EOF) {
                putc(ch, out);
                if (ch == '\\') putc(getc(in), out);
            }
            if (ch == '"') putc(ch, out);
            prev = '"';
            continue;
        }

        if (!foundMain && ch == 'm' && !isalnum(prev) && prev != '_') {
            long pos = ftell(in);
            word[0] = 'm'; word[1] = getc(in); word[2] = getc(in); word[3] = getc(in); word[4] = '\0';
            int nextChar = getc(in);
            
            if (strcmp(word, "main") == 0 && !isalnum(nextChar) && nextChar != '_') {
                foundMain = 1;
                fprintf(out, "%s", word);
                putc(nextChar, out);
                prev = nextChar;
                continue;
            } else {
                fseek(in, pos, SEEK_SET);
                putc(ch, out);
                prev = ch;
                continue;
            }
        }

        if (!foundMain && ch == '#') {
            while ((ch = getc(in)) != '\n' && ch != EOF);
            putc('\n', out);
            prev = '\n';
        } else {
            putc(ch, out);
            prev = ch;
        }
    }
}

struct token getNextToken(FILE *fp, int *l, int *c) {
    struct token t;
    int ch, i = 0;

    while ((ch = fgetc(fp)) != EOF && isspace(ch)) {
        if (ch == '\n') { (*l)++; *c = 1; }
        else (*c)++;
    }

    if (ch == EOF) { t.r_num = -1; return t; }

    t.r_num = *l;
    t.c_num = *c;

    if (isalpha(ch) || ch == '_') {
        t.lexeme[i++] = ch;
        while (isalnum(ch = fgetc(fp)) || ch == '_')
            t.lexeme[i++] = ch;
        ungetc(ch, fp);
    }
    else if (isdigit(ch)) {
        t.lexeme[i++] = ch;
        while (isdigit(ch = fgetc(fp)))
            t.lexeme[i++] = ch;
        ungetc(ch, fp);
    }
    else {
        t.lexeme[i++] = ch;
    }

    t.lexeme[i] = '\0';
    (*c) += i;
    return t;
}

int main() {
    FILE *fp, *step1, *step2;
    char file[100];
    int line = 1, col = 1;

    for (int i = 0; i < TABLE_SIZE; i++) hashTable[i] = NULL;

    printf("Input file: ");
    scanf("%s", file);
    fp = fopen(file, "r");
    if (!fp) { printf("Error opening file\n"); return 0; }

    step1 = fopen("p1.txt", "w+");
    stripComments(fp, step1);
    fseek(step1, 0, SEEK_SET);

    step2 = fopen("p2.txt", "w+");
    stripDirectives(step1, step2);
    fseek(step2, 0, SEEK_SET);

    struct token t, id, nxt;
    char currType[20] = "";

    while (1) {
        t = getNextToken(step2, &line, &col);
        if (t.r_num == -1) break;

        if (isType(t.lexeme)) {
            strcpy(currType, t.lexeme);
            
            while (1) {
                id = getNextToken(step2, &line, &col);
                if (id.r_num == -1) break;

                if (strcmp(id.lexeme, "main") == 0) {
                     insert("main", "", 0, "void");
                     while (1) {
                         nxt = getNextToken(step2, &line, &col);
                         if (!strcmp(nxt.lexeme, "{") || nxt.r_num == -1) break;
                     }
                     break; 
                }

                if (isKeyword(id.lexeme)) break; 

                nxt = getNextToken(step2, &line, &col);

                if (!strcmp(nxt.lexeme, "(")) {
                    insert(id.lexeme, "", 0, currType);
                    while (strcmp(nxt.lexeme, ")") != 0 && nxt.r_num != -1) 
                        nxt = getNextToken(step2, &line, &col);
                    nxt = getNextToken(step2, &line, &col); 
                }
                else if (!strcmp(nxt.lexeme, "[")) {
                    struct token num = getNextToken(step2, &line, &col);
                    getNextToken(step2, &line, &col); 
                    insert(id.lexeme, currType, atoi(num.lexeme) * typeSize(currType), "");
                    nxt = getNextToken(step2, &line, &col); 
                }
                else {
                    insert(id.lexeme, currType, typeSize(currType), "");
                }

                if (!strcmp(nxt.lexeme, ",")) continue; 
                else if (!strcmp(nxt.lexeme, ";")) break; 
                else break; 
            }
        }
    }

    printf("%s %s %s %s\n", "Lexeme", "Type", "Return", "Size");
    for (int i = 0; i < TABLE_SIZE; i++) {
        struct SymbolEntry *e = hashTable[i];
        while (e) {
            printf("%s %s %s %d\n", e->lexeme, e->type, e->returnType, e->size);
            e = e->next;
        }
    }

    fclose(fp);
    fclose(step1);
    fclose(step2);
    return 0;
}