
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h> 
#include <sys/wait.h>

int countNewlinesWithSpace(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int newlineCount = 0;
    int flag=0;
    int character;

    while ((character = fgetc(file)) != EOF) {
        if(character==32){
            flag=1;
        }
        else if(flag==1 && character==10){
            newlineCount++;
            //flag=0;
        }
        else if(character==10){
            flag=1;
        }
        else{
            flag=0;
        }
    }

    fclose(file);
    return newlineCount;
}

void countWords(const char *file1, int n_flag) {
    int count = 0;
    int word_flag=0;
    int chr; 
    int count_1=0;
    FILE *file = fopen(file1, "r");
    while ((chr = fgetc(file)) != EOF) {
        // printf("%d\n",chr);
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
        printf("The number of words are: %d\n",count);

    }
    else{
        printf("The number of words are: %d\n",count+countNewlinesWithSpace(file1));
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
        return count-countNewlinesWithSpace(file1);

    }
    else{
        return count;
    }
}
int main(int argc,char* argv[]) {
    while(1){
        char input[1000];
        printf("$ ");
        char currenDir[PATH_MAX];

        if((getcwd(currenDir, sizeof(currenDir)) != NULL)){
            printf("%s: ",currenDir);   
        }
        else{
            perror("Error:getcwd error");
        }
        
        if (!fgets(input, sizeof(input), stdin)) {
            exit(0);
        }
        //printf("%s",input);
        input[strcspn(input, "\n")] = '\0';
        if(input[0]=='\0'){
            continue;
        }
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
        if(strcmp(command, "exit")==0){
            break;
        }
        int rc=fork();
        if(rc<0){
            printf("fork failed");
        }
        else if (rc==0){
            if(strcmp(command, "word") == 0) {
                int n_flag = 0;
                int d_flag = 0;
                int wrong_flag=0;
                for (int i = 0; i < arg_count; i++) {
                    if (strcmp(arguments[i], "-n") == 0) {
                        n_flag = 1;
                    }
                    else if (strcmp(arguments[i], "-d") == 0) {
                        d_flag = 1;
                    }
                    else{
                        if(arguments[i][0]=='-'){
                            wrong_flag=1;
                        }
                    }
                }
                //printf("%d\n",wrong_flag);
                if(wrong_flag){
                    printf("wrong option given\n");
                }
                else{
                    if(d_flag==1 && n_flag == 1 ){
                        printf("used more than 1 flag\n");
                        exit(1);
                    }
                    if (d_flag) {
                        FILE *file1 = fopen(arguments[arg_count - 2], "r");
                        FILE *file2 = fopen(arguments[arg_count - 1], "r");
                        if (file1 && file2) {
                            int count1 = countWordsInFile(arguments[arg_count - 2], n_flag)+countNewlinesWithSpace(arguments[arg_count-2]);
                            int count2 = countWordsInFile(arguments[arg_count - 1], n_flag)+countNewlinesWithSpace(arguments[arg_count-1]);
                            printf("Difference in word count: %d\n", abs(count1 - count2));
                            fclose(file1);
                            fclose(file2);
                        } 
                        else {
                            printf("File not found\n");
                        }
                    } 
                    else if(n_flag) {
                        FILE *file = fopen(arguments[arg_count - 1], "r");
                        if (file){
                            countWords(arguments[arg_count - 1], n_flag);
                            fclose(file);
                        } 
                        else {
                            printf("File not found\n");
                        }
                    }
                    else{
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
            } 
            else if (strcmp(command, "dir") == 0) {
                int r_flag=0;
                int v_flag=0;
                int wrong_option_flag=0;
                for(int i=0;i<arg_count-1;i++){
                    if(strcmp(arguments[i], "-r")==0){
                        r_flag=1;
                    }
                    else if(strcmp(arguments[i],"-v")==0){
                        v_flag=1;
                    }
                    else{
                        printf("wrong option given\n");
                        wrong_option_flag=1;
                    }
                }
                if(wrong_option_flag){
                    continue;
                }
                if(r_flag == 1 && v_flag == 1){
                    printf("Error:more than 1 options used\n");
                    exit(1);
                }
                char* directory[5];
                directory[0] = "/home/sahilg/Desktop/OS-assignments-/operating_system/Assignment-1/Question2/./directory";
                directory[1] = arguments[arg_count-1];
                directory[2] = v_flag ? "1" : "0";
                directory[3] = r_flag ? "1" : "0";
                directory[4] = NULL;
            
                execvp(directory[0],directory);
                perror("execvp failed");
                exit(1);
            }
            else if (strcmp(command, "date")==0){
                int d1_flag=0;
                int R_flag=0;
                char* day = NULL;
                int wrong_option_flag=0;
                for(int i=0;i<arg_count-1;i++){
                    if(strcmp(arguments[i], "-d")==0){
                        d1_flag=1;
                    }
                    else if(strcmp(arguments[i], "-R")==0){
                        R_flag=1;
                    }
                    else if(arguments[i][0]!='-'){
                        if(d1_flag && day==NULL){
                            day=arguments[i];
                        }
                        else {
                            printf("arguments are either too less or too many\n");
                            wrong_option_flag=1;
                            break;
                        }
                    }
                    else{
                        printf("wrong option given\n");
                        wrong_option_flag=1;
                    }
                }
                if(wrong_option_flag){
                    continue;
                }
                if(d1_flag==1 && R_flag==1){
                    printf("used more than 1 flag\n");
                    exit(1);
                }
                if(day==NULL){
                    day="asdf";
                }
                char* directory[6];
                directory[0] = "/home/sahilg/Desktop/OS-assignments-/operating_system/Assignment-1/Question2/./date";
                directory[1] = arguments[arg_count-1];
                directory[2] = d1_flag ? "1" : "0";
                directory[3] = R_flag  ? "1":"0";
                directory[4]=day;
                directory[5]=NULL;
                execvp(directory[0], directory);
                perror("execvp failed");
                exit(1);
            
            }
            else{
                printf("Error : wrong command given : %s\n",command);
            }
            exit(0);
        }   
        else {
            wait(NULL);
            if(strcmp(command, "dir")==0){
                chdir(arguments[arg_count-1]);
            }
            //char *word_args[2];
              //  word_args[0] = "/home/sahilg/Desktop/OS-assignments-/operating_system/Assignment-1/Question2/./shell";
                //word_args[1] = NULL;
            //execvp(word_args[0], word_args);
        }
    }    
    return 0;
}