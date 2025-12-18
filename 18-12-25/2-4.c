#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void squeeze(char* s1, char* s2){
    char set[256] = {0};
    for(int i=0;s2[i]!='\0';i++){
        set[(unsigned char)s2[i]] = 1;
    }
    int ptr=0;
    for(int i=0;s1[i]!='\0';i++){
        if(set[(unsigned char)s1[i]]==0){
            s1[ptr++] = s1[i];
        }
        
    }
    s1[ptr] = '\0';
}

int main(){

    char str1[] = "";
    char str2[] = "";
    squeeze(str1,str2);
    printf("%s\n",str1);
    return 0;
}