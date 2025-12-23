// Exercise 4-7. Write a routine ungets(s) that will push back an entire string onto the input.
// Should ungets know about buf and bufp, or should it just use ungetch?

#include<stdio.h>
#include<string.h>
#define MAXBUFSIZE 100

char buf[MAXBUFSIZE];
int bufptr=0;

int getch(){
    return (bufptr>0) ? buf[--bufptr] : getchar();
}

void ungetch(int c){
    if(bufptr>=MAXBUFSIZE){
        printf("too many characters in the buffer\n");
    }else{
        buf[bufptr++] = c;
    }
}

void ungets(char s[]){
    for(int i=strlen(s)-1;i>=0;i--){
        ungetch(s[i]);
    }
}


int main(){


    ungets("nirmal");

    printf("%c\n",getch());
    printf("%c\n",getch());
    printf("%c\n",getch());
    printf("%c\n",getch());




    return 0;
}