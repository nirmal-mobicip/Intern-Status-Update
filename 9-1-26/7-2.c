// Exercise 7-2. Write a program that will print arbitrary input in a sensible way. As a
// minimum, it should print non-graphic characters in octal or hexadecimal according to local
// custom, and break long text lines.

#include<stdio.h>


int main(){
    int c;
    int col=0;
    int max_col = 20;

    while((c=getchar())!=EOF){
        if((c>=0 && c<=31) || c==127){
            printf("\\%03o",c);
            col+=4;
        }else{
            putchar(c);
            col++;
        }
        if(c=='\n'){
            putchar('\n');
            col=0;
        }
        if(col>max_col){
            putchar('\n');
            col = 0;
        }
    }

    return 0;
}