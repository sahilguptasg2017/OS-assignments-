#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

void delete_directory(const char *dir_name) {
    DIR *dir = opendir(dir_name);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char path[PATH_MAX];
                snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

                struct stat st;
                if (stat(path, &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        delete_directory(path); 
                    } 
                    else {
                        remove(path); 
                    }
                }
            }
        }
        closedir(dir);
    }

    if (rmdir(dir_name) == -1) {
        perror("Error removing directory");
        exit(1);
    }
}

void cd(const char *dir_name, int v_flag, int r_flag) {
    struct stat st;
    if (stat(dir_name, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (r_flag) {
                delete_directory(dir_name);
            } 
            else {
                printf("Directory already exists: %s\n", dir_name);
                exit(1);
            }
        } else {
            fprintf(stderr, "%s is not a directory\n", dir_name);
            exit(1);
        }
    }

    if (mkdir(dir_name, 0777) == -1) {
        perror("Error creating directory");
        exit(1);
    }

    if (v_flag) {
        printf("Directory created : %s\n", dir_name );
    }
    
    if (chdir(dir_name) == -1) {
        perror("Error changing directory");
        exit(1);
    }

    if (v_flag) {
        char currenDir[PATH_MAX];
        if(getcwd(currenDir,sizeof(currenDir))!=NULL){
            printf("path changed to: %s\n",currenDir);
        }
        else{
            perror("getcwd error");
        }
        //printf("Changed to directory: %s\n", dir_name);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <dir_name> <v_flag> <r_flag>\n", argv[0]);
        return 1;
    }

    const char* dir_name = argv[1];
    int v_flag = atoi(argv[2]);
    int r_flag = atoi(argv[3]);

    cd(dir_name, v_flag, r_flag);

    return 0;
}
