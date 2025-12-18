/*Exercise 1-10. Write a program to copy its input to its output, replacing each tab by \t, each
backspace by \b, and each backslash by \\. This makes tabs and backspaces visible in an
unambiguous way.*/

#include<stdio.h>

int main(){
    char ch;
    
    while((ch=getchar())!=EOF){
        if(ch=='\t'){                                 // When tab is encountered , it must print '\t'
            putchar('\\');
            putchar('t');
        }else if(ch=='\\'){                           // When backslash(\) is encountered , it must print '\\'
            putchar('\\');
            putchar('\\');    
        }else if(ch=='\b'){                           // When backspace(Ctrl+H sends control signal as backspace) is encountered , it must print '\b'
            putchar('\\');
            putchar('b');                             // I found the error in this code , previously it was '\b'
        }
        else{
            putchar(ch);
        }
    }

    printf("nirmal\rhelo\n");

    return 0;
}


