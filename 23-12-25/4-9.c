// Exercise 4-9. Our getch and ungetch do not handle a pushed-back EOF correctly. Decide
// what their properties ought to be if an EOF is pushed back, then implement your design.


#include<stdio.h>

#define MAXBUFSIZE 100

char buf[MAXBUFSIZE];
int bufptr=0;
int eof_flag=0;

int getch(){
    if(bufptr>0){
        int c = buf[--bufptr];
        if(c==EOF){
            eof_flag = 0;
        }
        return c;
    }else{
        return getchar();
    }
}

void ungetch(int c){
    if(bufptr>=MAXBUFSIZE){
        printf("too many characters in the buffer\n");
    }else{
        if(c==EOF){
            if(!eof_flag){
                eof_flag = 1;
            }else{
                printf("Invalid Operation\n");
                return;
            }
        }
        buf[bufptr++] = c;
    }
}

int main(){

    // while(1){

    //     ungetch(getch());
    //     ungetch(getch());
    //     ungetch(getch());

    //     printf("%c\n",getch());
    //     printf("%c\n",getch());
    //     printf("%c\n",getch());


    // }

    ungetch(EOF);
    getch();
    ungetch(EOF);
    ungetch('a');
    ungetch('b');
    
    

    printf("%c\n",getch());
    return 0;
}