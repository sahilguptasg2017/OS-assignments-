    for(int i=0;i<3;i++){
        int status;
        int wc = waitpid(rc_1, &status, 0);
        clock_gettime(CLOCK_MONOTONIC,&end); 
        long time_spent_seconds = end.tv_sec - start.tv_sec ; 
        long time_spent_nseconds = end.tv_nsec -start.tv_nsec;
        if(time_spent_nseconds < 0){
            time_spent_seconds-=1;
            time_spent_nseconds+=1000000000;
        }
        printf("%d %ld.%ld\n",i,time_spent_seconds,time_spent_nseconds);        
    }
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC,&start);
