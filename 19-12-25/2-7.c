// Exercise 2-7. Write a function invert(x,p,n) that returns x with the n bits that begin at
// position p inverted (i.e., 1 changed into 0 and vice versa), leaving the others unchanged.

#include <stdio.h>

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

int invert(unsigned int a, int p, int n){
    return a ^ (~(~0<<n) << (p+1-n));                                   // xor by 1 will toggle the bit
}

int main(){

    unsigned int a = 15;
    printBinary(a);
    int b = invert(a,3,3);
    printBinary(b);
    return 0;
}