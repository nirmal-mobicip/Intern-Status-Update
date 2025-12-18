/*Exercise 1-11. How would you test the word count program? What kinds of input are most
likely to uncover bugs if there are any?
Answer : test with empty lines, only spaces, and also no input

*/

#include<stdio.h>
#define OUT 0
#define IN 1

int main(){

    char ch;
    int state = OUT;
    int cnt=0;
    
    while((ch=getchar())!=EOF){
        
        if(ch!=' ' && ch!='\n' && ch!='\t'){       // when the input character is not space or tab or newline, then state is inside the word
            if(state!=IN) state = IN;
        }else{                                     // else the state is outside the word.
            if(state!=OUT){                        // I have updated the code in this line
                cnt++;                             // when state is changed to out , then means the word is crossed and thus increase the count.
                state = OUT;
            }
        }
        // if(state==OUT){
        //     cnt++;                              // I have made an error here in this code.
        // }



    }
    printf("No of Words : %d\n",cnt);

    return 0;
}