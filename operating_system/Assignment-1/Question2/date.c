#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

void date(const char *file_name, int time, int rfc) {
    struct stat file_stat;

    if (stat(file_name, &file_stat) == -1) {
        printf("File not found\n");
        return;
    }

    if (time) {
        printf("Time described by STRING: %s", asctime(localtime(&file_stat.st_mtime)));
    } else if (rfc) {
        char butt[80];
        strftime(butt, sizeof(butt), "%a, %d %b %Y %T %z", localtime(&file_stat.st_mtime)); // local time mainly work krta hai on the basis of time zone
        printf("Date and time in RFC 5322 format: %s\n", butt);
    } else {
        printf("Last modified date and time: %s", asctime(localtime(&file_stat.st_mtime)));
    }
}

int main(int argc, char* argv[]) {
    const char* fl_name = argv[1];
    int dspl = atoi(argv[2]);
    int rfc = atoi(argv[3]);
    date(fl_name,dspl,rfc);
    return 0;
}