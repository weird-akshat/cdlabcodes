#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct token {
    char lexeme[100];
    int sym_index;
    unsigned int r_num, c_num;
    char category[100];
};

int checkKeyword(char str[]) {
    char *list[] = {"main", "int", "char", "float", "double", "if", "else", 
                    "while", "for", "return", "void", "do", "switch", 
                    "case", "break", "continue", "const", "default", "sizeof"};
    int num_keywords = 19;
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, list[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int checkArithmetic(char ch) {
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') return 1;
    return 0;
}

int checkRelational(char *str) {
    char *ops[] = {"==", "!=", "<=", ">=", "<", ">"};
    for (int i = 0; i < 6; i++) {
        if (strcmp(str, ops[i]) == 0) return 1;
    }
    return 0;
}

int checkSpecial(char ch) {
    if (ch == '(' || ch == ')' || ch == '{' || ch == '}' ||
        ch == '[' || ch == ']' || ch == ',' || ch == ';') return 1;
    return 0;
}

void stripComments(FILE *src, FILE *dest) {
    int curr, next;
    while ((curr = getc(src)) != EOF) {
        if (curr == '"') {
            putc(curr, dest);
            while ((curr = getc(src)) != '"' && curr != EOF) {
                putc(curr, dest);
                if (curr == '\\') {
                    curr = getc(src);
                    putc(curr, dest);
                }
            }
            if (curr == '"') putc(curr, dest);
            continue;
        }

        if (curr == '/') {
            next = getc(src);
            if (next == '/') {
                while ((curr = getc(src)) != '\n' && curr != EOF);
                if (curr == '\n') putc('\n', dest);
            } 
            else if (next == '*') {
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
            } 
            else {
                putc(curr, dest);
                fseek(src, -1, SEEK_CUR);
            }
        } 
        else {
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
                if (ch == '\\') {
                    ch = getc(in);
                    putc(ch, out);
                }
            }
            if (ch == '"') putc(ch, out);
            prev = '"';
            continue;
        }

        if (!foundMain && ch == 'm' && !isalnum(prev) && prev != '_') {
            long pos = ftell(in);
            word[0] = 'm';
            word[1] = getc(in);
            word[2] = getc(in);
            word[3] = getc(in);
            word[4] = '\0';
            
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
        } 
        else {
            putc(ch, out);
            prev = ch;
        }
    }
}

struct token getNextToken(FILE *stream, int *ln, int *cl) {
    int ch;
    struct token tkn;
    tkn.r_num = 0;

    while ((ch = fgetc(stream)) != EOF) {
        if (ch == '\n') {
            (*ln)++;
            *cl = 1;
            continue;
        }
        if (isspace(ch)) {
            if (ch == '\t') *cl += 4;
            else (*cl)++;
            continue;
        }

        tkn.r_num = *ln;
        tkn.c_num = *cl;
        int pos = 0;

        if (isalpha(ch) || ch == '_') {
            tkn.lexeme[pos++] = ch;
            while (isalnum(ch = fgetc(stream)) || ch == '_') {
                tkn.lexeme[pos++] = ch;
            }
            fseek(stream, -1, SEEK_CUR);
            tkn.lexeme[pos] = '\0';
            strcpy(tkn.category, checkKeyword(tkn.lexeme) ? "Keyword" : "Identifier");
            *cl += pos;
            return tkn;
        }

        if (isdigit(ch)) {
            tkn.lexeme[pos++] = ch;
            while (isdigit(ch = fgetc(stream)) || ch == '.') {
                tkn.lexeme[pos++] = ch;
            }
            fseek(stream, -1, SEEK_CUR);
            tkn.lexeme[pos] = '\0';
            strcpy(tkn.category, "Number");
            *cl += pos;
            return tkn;
        }

        if (ch == '"') {
            tkn.lexeme[pos++] = ch;
            while ((ch = fgetc(stream)) != '"' && ch != EOF) {
                if (ch == '\n') (*ln)++;
                tkn.lexeme[pos++] = ch;
            }
            tkn.lexeme[pos++] = '"';
            tkn.lexeme[pos] = '\0';
            strcpy(tkn.category, "StringLiteral");
            *cl += pos;
            return tkn;
        }

        if (ch == '&' || ch == '|') {
            tkn.lexeme[pos++] = ch;
            int next = fgetc(stream);
            if (ch == next) {
                tkn.lexeme[pos++] = next;
                tkn.lexeme[pos] = '\0';
                strcpy(tkn.category, "LogicalOperator");
            } else {
                fseek(stream, -1, SEEK_CUR);
                tkn.lexeme[pos] = '\0';
                strcpy(tkn.category, "BitwiseOperator");
            }
            *cl += pos;
            return tkn;
        }

        if (ch == '=' || ch == '!' || ch == '<' || ch == '>') {
            tkn.lexeme[pos++] = ch;
            int next = fgetc(stream);
            if (next == '=') {
                tkn.lexeme[pos++] = next;
                tkn.lexeme[pos] = '\0';
                strcpy(tkn.category, "RelationalOperator");
            } else {
                fseek(stream, -1, SEEK_CUR);
                tkn.lexeme[pos] = '\0';
                if (ch == '=') strcpy(tkn.category, "AssignmentOp");
                else if (ch == '!') strcpy(tkn.category, "LogicalOperator");
                else strcpy(tkn.category, "RelationalOperator");
            }
            *cl += pos;
            return tkn;
        }

        if (checkArithmetic(ch)) {
            tkn.lexeme[0] = ch;
            tkn.lexeme[1] = '\0';
            strcpy(tkn.category, "ArithmeticOperator");
            (*cl)++;
            return tkn;
        }

        if (checkSpecial(ch)) {
            tkn.lexeme[0] = ch;
            tkn.lexeme[1] = '\0';
            strcpy(tkn.category, "SpecialSymbol");
            (*cl)++;
            return tkn;
        }
    }
    tkn.r_num = -1;
    return tkn;
}

int main() {
    FILE *input, *step1, *step2;
    char path[100];
    int line = 1, column = 1;
    struct token current;

    printf("Input file: ");
    scanf("%s", path);
    input = fopen(path, "r");
    if (!input) {
        printf("Error opening %s\n", path);
        return -1;
    }

    step1 = fopen("p1.txt", "w+");
    stripComments(input, step1);
    fseek(step1, 0, SEEK_SET);

    step2 = fopen("p2.txt", "w+");
    stripDirectives(step1, step2);
    fseek(step2, 0, SEEK_SET);

    while (1) {
        current = getNextToken(step2, &line, &column);
        if (current.r_num == -1) break;
        printf("Row: %d Col: %d Lexeme: %s Type: %s\n", 
               current.r_num, current.c_num, current.lexeme, current.category);
    }

    fclose(input);
    fclose(step1);
    fclose(step2);
    return 0;
}