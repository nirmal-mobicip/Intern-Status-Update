#include<stdio.h>

void printB(int val)
{
    if(val>1){
        printB(val>>1);
    }
    printf("%d",val&1);
}

void printBinary(int val){
    printB(val);
    printf("\n");
}
