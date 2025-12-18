/*Exercise 1-20. Write a program detab that replaces tabs in the input with the proper number
of blanks to space to the next tab stop. Assume a fixed set of tab stops, say every n columns.
Should n be a variable or a symbolic parameter?*/

#include<stdio.h>
#include<limits.h>

#define TAB 4

int getLine(char line[]){
    int len=0;
    char ch = getchar();
    while(ch!=EOF && ch!='\n'){
        line[len++] = ch;
        ch = getchar();
    }
    return len;
}

void printLine(char line[],int len){
    for(int i=0;i<len;i++){
        putchar(line[i]);
    }
    printf("\n");
}


void detab(char line[],int len){
    for(int i=0;i<len;i++){
        if(line[i]=='\t'){
            for(int j=0;j<TAB;j++)  putchar(' ');
        }else{
            putchar(line[i]);
        }
    }
}


int main(){

    char current_line[100];
    int len = 1;
    while(len>0){
        len = getLine(current_line);
        detab(current_line,len);
    }
    return 0;
}