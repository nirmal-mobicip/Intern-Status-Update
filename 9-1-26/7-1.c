// Exercise 7-1. Write a program that converts upper case to lower or lower case to upper,
// depending on the name it is invoked with, as found in argv[0].

#include<stdio.h>
#include<ctype.h>

int main(){

    int c;
    while((c = getchar())!=EOF){                                // for lower
        putchar(tolower(c));
    }

    // int c;
    // while((c = getchar())!=EOF){                             // for upper
    //     putchar(toupper(c));
    // }
    
    return 0;
}


// cmd : ./lower < text.txt     converts to lower
// ./upper < text.txt           converts to upper
