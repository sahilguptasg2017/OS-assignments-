#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void createDirectory(const char *dir_name, int flag, int remove_if_exists) {
    if (mkdir(dir_name, 0777) == -1) {
        if (remove_if_exists) {
            if (flag) {
                printf("Removing existing directory: %s\n", dir_name);
            }
            if (rmdir(dir_name) == -1) {
                perror("Error removing directory");
                exit(1);
            }
            mkdir(dir_name, 0777);
        } else {
            printf("Directory already exists: %s\n", dir_name);
            exit(1);
        }
    }

    if (flag) {
        printf("Directory created: %s\n", dir_name);
    }
    
    // Change to the created directory
    if (chdir(dir_name) == -1) {
        perror("Error changing directory");
        exit(1);
    }
    if (flag) {
        printf("Changed to directory: %s\n", dir_name);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <dir_name> <flag> <remove_if_exists>\n", argv[0]);
        return 1;
    }

    const char* dir_name = argv[1];
    int flag = atoi(argv[2]);
    int remove_if_exists = atoi(argv[3]);

    createDirectory(dir_name, flag, remove_if_exists);

    return 0;
}