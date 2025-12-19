// Exercise 2-9. In a two's complement number system, x &= (x-1) deletes the rightmost 1-bit
// in x. Explain why. Use this observation to write a faster version of bitcount.

#include<stdio.h>
#include "base.h"

int countbits(int x){
    int cnt=0;
    while(x!=0){
        x&=(x-1);                       // it removes one 1 bit from x
        cnt++;                          // this statement is executed untill x becomes 0 and count incremented
    }
    return cnt;
}

int main(){

    unsigned int a = 16;
    printBinary(a);
    printf("%u\n",a);
    printf("No of 1's : %d\n",countbits(a));

    return 0;
}