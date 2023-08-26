#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void cd(const char *dir_name, int v_flag, int r_flag) {
    if (mkdir(dir_name, 0777) == -1) {
        if (r_flag) {
            if (v_flag) {
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

    if (v_flag) {
        printf("Directory created: %s\n", dir_name);
    }
    
    if (chdir(dir_name) == -1) {
        perror("Error changing directory");
        exit(1);
    }
    if (v_flag) {
        printf("Changed to directory: %s\n", dir_name);
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