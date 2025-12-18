/*Exercise 1-2. Experiment to find out what happens when prints's argument string contains
\c, where c is some character not listed above.*/
#include<stdio.h>

int main(){

    printf("\c");       // prints just the character 'c'  : warning: unknown escape sequence: '\c'
    printf("\\c");       // prints just the character "\c"
    return 0;
}