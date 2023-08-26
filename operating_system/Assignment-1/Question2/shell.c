#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <sys/wait.h>

int countn(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    int wordCount = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, " ");
        while (token != NULL) {
            //printf("%c\n",token[1]);
            if(token[1]=='n' && token[0]=='\\' && strlen(token)==2){
                wordCount++;
            }
            token = strtok(NULL, " ");
        }
    }

    fclose(file);
    return wordCount;
}

void countWords(const char *file1, int n_flag) {
    int count = 0;
    int word_flag=0;
    int chr; 
    int count_1=0;
    FILE *file = fopen(file1, "r");
    while ((chr = fgetc(file)) != EOF) {
        if(isspace(chr)){
            if(word_flag){
                count++;
                word_flag=0;
            }
        }
        else{
            word_flag=1;
        }
    // char line[1000]
    //  printf("Number of words: %d\n", count);
    }
    if(word_flag){
            count++;
    }
    if(n_flag){
        printf("The number of words are: %d\n",count-countn(file1));

    }
    else{
        printf("The number of words are: %d\n",count);
    }
}

int countWordsInFile(const char *file1, int n_flag) {
    int count = 0;
    int word_flag=0;
    int chr; 
    int count_1=0;
    FILE *file = fopen(file1, "r");
    while ((chr = fgetc(file)) != EOF) {
        if(isspace(chr)){
            if(word_flag){
                count++;
                word_flag=0;
            }
        }
        else{
            word_flag=1;
        }
    // char line[1000]
    //  printf("Number of words: %d\n", count);
    }
    if(word_flag){
            count++;
    }
    if(n_flag){
        return count-countn(file1);

    }
    else{
        return count;
    }
}
int main(int argc,char* argv[]) {
        char input[1000];
        printf("$ ");
        if (!fgets(input, sizeof(input), stdin)) {
            exit(0);
        }
        input[strcspn(input, "\n")] = '\0';

        char *token = strtok(input, " ");
        char *command = token;
        char *arguments[10];
        int arg_count = 0;
        while (token != NULL) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                arguments[arg_count++] = token;
            }
        }
        int rc=fork();
        if(rc<0){
            printf("fork failed");
        }
        else if (rc==0){
            if(strcmp(command, "word") == 0) {
                int n_flag = 0;
                int d_flag = 0;
                for (int i = 0; i < arg_count; i++) {
                    if (strcmp(arguments[i], "-n") == 0) {
                        n_flag = 1;
                    }if (strcmp(arguments[i], "-d") == 0) {
                        d_flag = 1;
                    }
                }
                if(d_flag==1 && n_flag == 1 ){
                    printf("used more than 1 flag\n");
                    exit(1);
                }
                if (d_flag) {
                    FILE *file1 = fopen(arguments[arg_count - 2], "r");
                    FILE *file2 = fopen(arguments[arg_count - 1], "r");
                    if (file1 && file2) {
                        int count1 = countWordsInFile(arguments[arg_count - 2], n_flag);
                        int count2 = countWordsInFile(arguments[arg_count - 1], n_flag);
                        printf("Difference in word count: %d\n", abs(count1 - count2));
                        fclose(file1);
                        fclose(file2);
                    } 
                    else {
                        printf("File not found\n");
                    }
                } 
                else {
                    FILE *file = fopen(arguments[arg_count - 1], "r");
                    if (file){
                        countWords(arguments[arg_count - 1], n_flag);
                        fclose(file);
                    } 
                    else {
                        printf("File not found\n");
                    }
                }
            } 
            else if (strcmp(command, "dir") == 0) {
                int remove_if_exists = 0;
                int flag = 0;
                for (int i = 0; i < arg_count; i++) {
                    if (strcmp(arguments[i], "-r") == 0) {
                        remove_if_exists = 1;
                    }
                    else if (strcmp(arguments[i], "-v") == 0) {
                        flag = 1;
                    }
                }
                const char *dir = arguments[arg_count - 1];
            //  createDirectory(dir, flag, remove_if_exists);
                chdir(dir);
            }
            else if (strcmp(command, "date")){


            }
        }   
        else {
            wait(NULL);
            char *word_args[2];
                word_args[0] = "./shell";
                word_args[1] = NULL;
            execvp(word_args[0], word_args);
        }
        return 0;
}