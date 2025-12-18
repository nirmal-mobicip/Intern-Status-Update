/*Exercise 1-21. Write a program entab that replaces strings of blanks by the minimum
number of tabs and blanks to achieve the same spacing. Use the same tab stops as for detab.
When either a tab or a single blank would suffice to reach a tab stop, which should be given
preference?*/

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


void entab(char line[],int len){
    int nos=0;
    for(int i=0;i<len;i++){
        if(line[i]==' '){
            nos++;
        }else{
            if(nos!=0){
                for(int i=0;i<nos;i++) putchar(' ');
            }
            nos = 0;
            putchar(line[i]);
        }
        if(nos==4){
            putchar('\t');
            nos = 0;
        }
    }
}


int main(){

    char current_line[100];
    int len = 1;
    while(len>0){
        len = getLine(current_line);
        entab(current_line,len);
    }
    return 0;
}