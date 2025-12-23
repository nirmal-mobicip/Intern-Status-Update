// Exercise 4-8. Suppose that there will never be more than one character of pushback. Modify
// getch and ungetch accordingly.

#include<stdio.h>

int buffer = '\0';
int hasbuf = 0;

int getch(){
    if(buffer){
        hasbuf = 0;
        return buffer;
    }else{
        return getchar();
    }
}

void ungetch(int c){
    if(hasbuf){
        printf("too many characters in the buffer\n");
    }else{
        buffer = c;
        hasbuf = 1;
    }
}


int main(){

    ungetch('a');
    ungetch('b');
    ungetch('c');

    printf("%c\n",getch());

    ungetch('b');
    printf("%c\n",getch());

    return 0;
}