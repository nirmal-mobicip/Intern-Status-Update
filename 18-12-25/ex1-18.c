/*Exercise 1-19. Write a function reverse(s) that reverses the character string s. Use it to
write a program that reverses its input a line at a time.*/

#include<stdio.h>
#include<string.h>
int getLine(char line[]){
    int len=0;
    int ch;
    while((ch=getchar())!=EOF && ch!='\n'){
        line[len++] = ch;
    }
    // line[len++] = '\n';
    line[len] = '\0';                     // updated the string with null terminator

    if(ch==EOF){
        return EOF;                       // if EOF is encountered then it is signalled
    }else{
        return 1;
    }
}

void printLine(char line[]){
    for(int i=0;i<line[i]!='\0';i++){
        putchar(line[i]);
    }
    printf("\n");
}

void reverseLine(char line[]){
    int len = strlen(line);
    for(int i=0;i<(len/2);i++){                     // traverse the string line upto the half and swap it with the last , then last before and so on.
        char temp = line[i];
        line[i] = line[len-i-1];
        line[len-i-1] = temp;
    }
}


int main(){

    char current_line[100];
    int status;
    while((status = getLine(current_line))!=EOF){
        printf("String : %s\n",current_line);
        printf("Length : %lu\n",strlen(current_line));
        reverseLine(current_line);
        printLine(current_line);
    }
    return 0;
}