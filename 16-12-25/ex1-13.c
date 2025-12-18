/*Exercise 1-13. Write a program to print a histogram of the lengths of words in its input. It is
easy to draw the histogram with the bars horizontal; a vertical orientation is more challenging.*/

#include<stdio.h>
#define OUT 0
#define IN 1

int main(){

    char ch;
    int state = OUT;
    int len=0;
    int hist[10] = {0};
    while((ch=getchar())!=EOF){
        
        if(ch!=' ' && ch!='\n' && ch!='\t'){
            if(state!=IN){
                state = IN;   
            }
            len++;                                      // when non-space-character is encountered len increments
        }else{
            if(state!=OUT){
                hist[len]++;
                len=0;                                  // when space-charater encountered , len is updated and resetted to 0 and state is updated to out.
                state = OUT;
            }
        }
        // if(state==OUT){
        //     hist[len]++;                             // similar mistake as previous.
        //     len=0;
        // }
    }

    for(int i=0;i<10;i++){
        printf("%d",i);
        for(int j=0;j<hist[i];j++){
            printf("*");
        }
        printf("\n");
    }

    return 0;
}