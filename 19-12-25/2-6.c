// Exercise 2-6. Write a function setbits(x,p,n,y) that returns x with the n bits that begin at
// position p set to the rightmost n bits of y, leaving the other bits unchanged.

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

int getbits(unsigned int a, int p, int n)
{
    int res = 0;
    if (p + 1 >= n)
    {
        a >>= (p + 1 - n);
        res = a & ~(~0 << n);
    }
    return res;
}

int setbits(unsigned int a, int p, int n, unsigned int y)
{

    if (p < BITS && n < p + 1)
    {
        int val = ~(~0 << BITS) << n;
        for (int i = 0; i < (p + 1 - n); i++)
        {
            val = (val << 1) | 1;                                       
        }                                                   // creates a mask
        val = a & val;                                      // set 0 to the bits to be changed 
        // printBinary(val);
        y <<= (p + 1 - n);
        val = y | val;                                      // now perform or with the val and y to store the bits in y to that position
        return val;
    }
    else
    {
        return -1;
    }
}

int main()
{

    unsigned int a = 100;
    printBinary(a);
    int b = setbits(a, 5, 3, 3);
    printBinary(b);
    return 0;
}