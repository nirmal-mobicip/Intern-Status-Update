// Exercise 4-14. Define a macro swap(t,x,y) that interchanges two arguments of type t.
// (Block structure will help.)



#include<stdio.h>

#define swap(t,x,y)        \
            do{               \
            t _temp = (x);   \
            (x) = (y);      \
            (y) = _temp;     \
            }while(0) 

int main(){

    int a = 57;
    int b = 6;

    if(a>b){
        swap(int,a,b);
    }else{
        printf("No swap\n");
    }
    printf("%d,%d\n",a,b);

    return 0;
}