// Exercise 2-10. Rewrite the function lower, which converts upper case letters to lower case,
// with a conditional expression instead of if-else.

#include<stdio.h>

int my_tolower(int c){
    return (c>='A' && c<='Z')?(c-'A'+'a'):c;                // ?: syntax is used instead of if else
}

int main(){

    printf("%c\n",my_tolower('Z'));

    return 0;
}