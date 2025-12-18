/*Exercise 1-16. Revise the main routine of the longest-line program so it will correctly print
the length of arbitrary long input lines, and as much as possible of the text.*/

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

void copy(char from[], char to[]){
    int i;
    for(i=0;from[i]!='\0';i++){
        to[i] = from[i];
    }
    to[i] = '\0';
}

int main(){

    char longest_line[100];
    int longest_length = INT_MIN;

    char current_line[100];
    int len = 1;
    while(len>0){
        len = getLine(current_line);
        if(len>longest_length){
            longest_length = len;
            copy(current_line,longest_line);
        }
    }

    printf("Longest Line : %s\n",longest_line);
    printf("Longest Length : %d\n",longest_length);
    

    return 0;
}