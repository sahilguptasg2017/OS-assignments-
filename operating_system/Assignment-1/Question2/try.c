#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int countWordsInFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int wordCount = 0;
    char buffer[1024]; // Buffer to store each line
    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, " "); // Split line into words using space as delimiter
        while (token != NULL) {
            printf("%c\n",token[1]);
            if(token[1]=='n' && token[0]=='\\' ){
                wordCount++;
            }
            token = strtok(NULL, " ");
        }
    }

    fclose(file);
    return wordCount;
}
int main() {
    const char *filename = "/home/sahilg/OS/OS-assignments-/operating_system/Assignment-1/Question2/text.txt";
    int result = countWordsInFile(filename);

    if (result >= 0) {
        printf("Number of words \\n characters: %d\n", result);
    }

    return 0;
}