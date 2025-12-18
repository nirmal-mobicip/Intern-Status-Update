/*Exercise 1-17. Write a program to print all input lines that are longer than 80 characters.*/

#include<stdio.h>
#include<limits.h>

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


int main(){

    char current_line[100];
    int len = 1;
    while(len>0){
        len = getLine(current_line);
        if(len>=80){
            printLine(current_line,len);
        }
    }
    return 0;
}