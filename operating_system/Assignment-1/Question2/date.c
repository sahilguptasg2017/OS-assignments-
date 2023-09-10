#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

struct DateTime {
    int wk;      
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

struct DateTime date_change(struct stat f,const char* relative_time) {
    struct DateTime dt;
    time_t current_time = time(NULL);
    struct tm* local_time = localtime(&f.st_mtime);
    int y,u,o;
    if (strcmp(relative_time, "yesterday") == 0) {
        y=1; 
    } else if (strcmp(relative_time, "tomorrow") == 0) {
        u=1; 
    } else if (strcmp(relative_time, "last year") == 0) {
        o=1;
    } 
    struct tm* adjusted_time = localtime(&f.st_mtime);
    dt.year = adjusted_time->tm_year + 1900;
    if (o){
    	dt.year--;
    	}
    dt.month = adjusted_time->tm_mon + 1;
    if (y){
    	dt.wk = adjusted_time->tm_wday;
    	dt.wk-=1;
        if(dt.wk<0){
            dt.wk = 6;
        }
    	dt.day = adjusted_time->tm_mday;
    	dt.day-=1;
    }
    else{
    	dt.wk = adjusted_time->tm_wday;
    	dt.wk=adjusted_time->tm_mday;
    } 
    if (u){
    	dt.wk = adjusted_time->tm_wday;
    	dt.wk+=1;
        if(dt.wk>6){
            dt.wk = 0;
        }
    	dt.day = adjusted_time->tm_mday;
    	dt.day+=1;
    }
    dt.hour = adjusted_time->tm_hour;
    dt.minute = adjusted_time->tm_min;
    dt.second = adjusted_time->tm_sec;
    
    return dt;
}

void date(const char* file_name, int time, int rfc, const char* a) {
    struct stat file_stat;
    struct DateTime dt;
    char buffer[10002];
    
    if (stat(file_name, &file_stat) == -1) {
        printf("File not found\n");
        return;
    }
    
    if (time && (strcmp(a,"now")==0|| strcmp(a,"asdf")==0)){
    	printf("Time described by STRING: %s", asctime(localtime(&file_stat.st_mtime)));
    	return;    	
    } 
    
    if (time) {
        if (strcmp(a, "now") == 0) {
              dt.year = file_stat.st_mtime;
            dt.month = file_stat.st_mtime;
            //dt.wk=file_stat.st_mtime;
            dt.day = file_stat.st_mtime;
            dt.hour = file_stat.st_mtime;
            dt.minute = file_stat.st_mtime;
            dt.second = file_stat.st_mtime;
        } else {
            dt = date_change(file_stat,a);
        }
        char* s=asctime(localtime(&file_stat.st_mtime));
        // Create an array to store day names
        const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

        printf("Modified date and time in STRING: %s %02d %s %04d %02d:%02d:%02d\n",
               days[dt.wk], dt.day, (dt.month == 1)?"Jan":(dt.month == 2)?"Feb":(dt.month == 3)?"Mar":(dt.month == 4)?"Apr":(dt.month == 5)?"May":(dt.month == 6)?"Jun":(dt.month == 7)?"Jul":(dt.month == 8)?"Aug":(dt.month == 9)?"Sep":(dt.month == 10)?"Oct":(dt.month == 11)?"Nov":"Dec", dt.year, dt.hour, dt.minute, dt.second);
    }
    else if (rfc) {
        char butt[80];
        strftime(butt, sizeof(butt), "%a, %d %b %Y %T %z", localtime(&file_stat.st_mtime));
        printf("Date and time in RFC 5322 format: %s\n", butt);
    } else {
        printf("Time described: %s", asctime(localtime(&file_stat.st_mtime)));   
    }
}

int main(int argc, char* argv[]) {
    const char* fl_name = argv[1];
    int dspl = atoi(argv[2]);
    int rfc = atoi(argv[3]);
    char* relative_time = argv[4];
    date(fl_name, dspl, rfc, relative_time);
    return 0;
}