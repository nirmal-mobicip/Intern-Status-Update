/*Exercise 1-12. Write a program that prints its input one word per line.*/

#include<stdio.h>
#define OUT 0
#define IN 1

int main(){

    char ch;
    int state = OUT;
    
    while((ch=getchar())!=EOF){
        
        if(ch!=' ' && ch!='\n' && ch!='\t'){    // when the input character is not space or tab or newline, then state is inside the word
            state = IN;
            putchar(ch);
        }else{                                  // else the state is outside the word.
            if(state!=OUT){                     // I have updated the code in this line (similar mistake as previous)
                putchar('\n');                  // when state is changed to out , then means the word is crossed and thus increase the count.
                state = OUT;
            }
            
        }
        // if(state==OUT){   
        //     putchar('\n');                   // Same error as previous code.
        // }
        
    }

    return 0;
}