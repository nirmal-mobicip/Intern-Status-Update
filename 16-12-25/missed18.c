/*Exercise 1-18. Write a program to remove trailing blanks and tabs from each line of input,
and to delete entirely blank lines.*/

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
    if(len>0){
        for(int i=0;i<len;i++){
            putchar(line[i]);
        }
        printf("\n");
    }
}

void trimLine(char line[],int* len){
    if(len>0){
        int i=*len-1;
        while(i>=0 && (line[i]==' ' || line[i]=='\t')){
            i--;
        }
        *len = i+1;
    }
}


int main(){

    char current_line[100];
    int len;
    while(1){
        len = getLine(current_line);
        printf("Initial Length : %d\n",len);
        trimLine(current_line,&len);
        printLine(current_line,len);
        printf("Trimmed Length : %d\n",len);
    }
    return 0;
}