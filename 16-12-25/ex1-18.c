/*Exercise 1-19. Write a function reverse(s) that reverses the character string s. Use it to
write a program that reverses its input a line at a time.*/

#include<stdio.h>

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

void reverseLine(char line[],int len){
    for(int i=0;i<(len/2);i++){
        char temp = line[i];
        line[i] = line[len-i-1];
        line[len-i-1] = temp;
    }
}


int main(){

    char current_line[100];
    int len = 1;
    while(len>0){
        len = getLine(current_line);
        printf("Length : %d\n",len);
        reverseLine(current_line,len);
        printLine(current_line,len);
    }
    return 0;
}