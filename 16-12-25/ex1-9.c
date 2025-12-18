/*Exercise 1-9. Write a program to copy its input to its output, replacing each string of one or
more blanks by a single blank.*/

#include<stdio.h>

int main(){
    char ch,prev=' ';
    
    while((ch=getchar())!=EOF){
        // printf("%c\n",ch);
        if(prev!=' ' && ch==' '){
            printf(" ");
        }
        else{
            if(ch!=' '){
                printf("%c",ch);
            }
        }
        prev = ch;
    }

    return 0;
}