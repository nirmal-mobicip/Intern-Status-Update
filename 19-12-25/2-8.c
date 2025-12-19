// Exercise 2-8. Write a function rightrot(x,n) that returns the value of the integer x rotated
// to the right by n positions.

#include <stdio.h>

#define BITS 8

void printB(int val)
{
    if (val > 1)
    {
        printB(val >> 1);
    }
    printf("%d", val & 1);
}

void printBinary(int val)
{
    printB(val);
    printf("\n");
}

int no_of_bits(unsigned int x)
{
    int len = 1;
    while (x > 1)
    {
        x >>= 1;
        len++;
    }
    return len;
}

int rightrot(unsigned int x)
{
    if((x&1)==0){
        return x>>1;
    }else{                                                          // rotate right by 1 bit
        x>>=1;
        int val = 1<<BITS;
        return x | val;
    }
}
int right_rotate(unsigned int x, int n){
    for(int i=0;i<n;i++){
        x = rightrot(x);                                            // loop to rotate by n bits  (loop iterates for n times)
        // printBinary(x);
    }
    return x;
}

int main()
{
    int a = 19;
    printBinary(a);
    printBinary(right_rotate(a,3));
    return 0;
}