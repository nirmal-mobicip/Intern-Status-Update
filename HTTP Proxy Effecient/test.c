#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(){

    char* str = (char*)malloc(100*sizeof(str));
    // strcat(str,"helo");
    // strcat(str,"helo");
    printf("**%s**\n",str);

    return 0;
}