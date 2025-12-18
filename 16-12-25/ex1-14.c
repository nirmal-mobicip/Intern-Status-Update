/*Exercise 1-14. Write a program to print a histogram of the frequencies of different characters
in its input.*/


#include<stdio.h>
#define OUT 0
#define IN 1

int main(){

    char ch;
    int hist[26] = {0};
    while((ch=getchar())!=EOF){
        if(ch!=' ' && ch!='\n' && ch!='\t'){
            hist[ch-'a']++;
        }   
    }
    for(int i=0;i<26;i++){
        printf("%d ",hist[i]);
    }
    putchar('\n');
    for(int i=0;i<26;i++){
        printf("%c ",i+'a');
    }
    putchar('\n');
    return 0;
}