/*Exercise 2-2. Write a loop equivalent to the for loop above without using && or ||.*/

#include <stdio.h>
#include <string.h>

#define LIMIT 5

int main()
{
    char s[LIMIT];
    int i=0,c;

    // for (i = 0; i < LIMIT - 1 && (c = getchar()) != '\n' && c != EOF; ++i){
    //     printf("%d - %c\n",i,c);
    //     s[i] = c;
    // }

    
    while(i<LIMIT-1){
        if((c=getchar())!='\n'){
            if(c!=EOF){
                printf("%d - %c\n",i,c);                            // this approach uses nested if statement in order to achieve loop equivalent without '&&' and '||'
                s[i] = c;
            }else{
                break;
            }
        }else{
            break;
        }
        ++i;
    }
    printf("%s\n",s);



    return 0;
}