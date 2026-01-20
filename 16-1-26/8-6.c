// Exercise 8-6. The standard library function calloc(n,size) returns a pointer to n objects of
// size size, with the storage initialized to zero. Write calloc, by calling malloc or by
// modifying it.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *my_calloc(unsigned n, unsigned size)
{

    void *p;
    unsigned total;

    if (n == 0 || size == 0)
    {
        return NULL;
    }

    if (n > (unsigned)-1 / size)
        return NULL;

    total = n * size;

    p = malloc(total);

    if (p == NULL)
    {
        return NULL;
    }
    memset(p, 0, total);
    return p;
}

int main()
{

    int* arr = (int*)calloc(10,sizeof(int));

    for(int i=0;i<10;i++){
        printf("%d ",arr[i]);
    }
    putchar('\n');


    return 0;
}