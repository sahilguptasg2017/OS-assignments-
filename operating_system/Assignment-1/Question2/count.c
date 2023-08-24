#include <stdio.h>
#include <stdlib.h>



void countWords(FILE *file, int n_flag) {
    int count = 0;
    int word_flag = 0;
    int c;
    while ((c = fgetc(file)) != EOF) {
        printf("%d\n",c);
        if (c == 32 || c == 9) {
            if(word_flag==1)
                count++;
            word_flag=0;
        } 
        else if (n_flag && (c == 110) && word_flag!=0) {
            count++;
            word_flag=0;
        }
    }
    if (word_flag) {
        count++;
    }
    printf("The number of words in the file is: %d\n", count);
}