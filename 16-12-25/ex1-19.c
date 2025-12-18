#include"base.h"
#include<stdio.h>

int main(){
    extern int external_variable;
    external_variable ++;
    printf("%d\n",external_variable);
    printf("%d\n",adminAdd(5,external_variable));
}