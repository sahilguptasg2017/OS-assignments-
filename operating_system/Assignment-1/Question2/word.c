#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count(char* text) {
    int word_counter = 0;
    int space_counter = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == ' ' || text[i] == '\t') {
            space_counter = 1;
        }
        else if (space_counter == 1) {
            word_counter++;
            space_counter = 0;
        }
    }
    return word_counter;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Too few arguments\n");
        exit(1);
    }

    int n_flag = 0;
    int d_flag = 0;

    if (strcmp(argv[1], "-n") == 0) {
        n_flag = 1;
    }
    else if (strcmp(argv[1], "-d") == 0) {
        d_flag = 1;
    }
    else {
        printf("Invalid option: %s\n", argv[1]);
        exit(1);
    }

    if (argc != (n_flag ? 3 : 4)) {
        printf("Usage: %s [-n] [-d] file1.txt [file2.txt]\n", argv[0]);
        exit(1);
    }

    char* file1 = argv[n_flag ? 2 : 3];
    char* file2 = argv[n_flag ? 3 : 4];

    FILE* fp1 = fopen(file1, "r");
    if (fp1 == NULL) {
        printf("Error: File '%s' does not exist.\n", file1);
        return 1;
    }

    int word_count1 = 0;
    int word_count2 = 0;
    int word_diff = 0;

    char line[1024];
    while (fgets(line, sizeof(line), fp1)) {
        word_count1 += count(line);
    }

    fclose(fp1);

    if (n_flag) {
        printf("Word count in '%s': %d\n", file1, word_count1);
    }
    else if (d_flag) {
        FILE* fp2 = fopen(file2, "r");
        if (fp2 == NULL) {
            printf("Error: File '%s' does not exist.\n", file2);
            return 1;
        }

        while (fgets(line, sizeof(line), fp2)) {
            word_count2 += count(line);
        }

        fclose(fp2);

        word_diff = abs(word_count1 - word_count2);
        printf("Word count difference: %d\n", word_diff);
    }

    return 0;
}
