#include<stdio.h>

int main(){
    int a = 1<<7;
    a = a | 3;
    printf("%d\n",a);
    return 0;
}