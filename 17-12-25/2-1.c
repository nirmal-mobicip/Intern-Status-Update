/*Exercise 2-1. Write a program to determine the ranges of char, short, int, and long
variables, both signed and unsigned, by printing appropriate values from standard headers
and by direct computation. Harder if you compute them: determine the ranges of the various
floating-point types.*/

#include<stdio.h>
#include<limits.h>


int main(){

    printf("Unsigned Char : %d - %d\n",0,UCHAR_MAX);
    printf("Signed Char : %d - %d\n",CHAR_MIN,CHAR_MAX);

    printf("Unsigned Short : %d - %d\n",0,USHRT_MAX);
    printf("Signed Short : %d - %d\n",SHRT_MIN,SHRT_MAX);

    printf("Unsigned Int : %d - %u\n",0,UINT_MAX);
    printf("Signed Int : %d - %d\n",INT_MIN,INT_MAX);

    printf("Unsigned Long : %lu - %lu\n",0UL,ULONG_MAX);
    printf("Signed Long : %ld - %ld\n",LONG_MIN,LONG_MAX);

    
    

    return 0;
}



// the limits.h library has all the extreme values of each data type which is demonstrated.