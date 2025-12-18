/*Exercise 1-10. Write a program to copy its input to its output, replacing each tab by \t, each
backspace by \b, and each backslash by \\. This makes tabs and backspaces visible in an
unambiguous way.*/

#include<stdio.h>

int main(){
    char ch;
    
    while((ch=getchar())!='\n'){
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

    printf("\n");

    return 0;
}




#include <stdio.h>
#include <termios.h>
#include <unistd.h>
struct termios orig_termios;

void enable_raw_mode() {
    struct termios raw;

    // Get current terminal attributes
    tcgetattr(STDIN_FILENO, &orig_termios);

    // Copy to modify
    raw = orig_termios;

    // Disable canonical mode and echo
    raw.c_lflag &= ~(ICANON | ECHO);

    // Apply immediately
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
int main() {
    enable_raw_mode();  // turn on raw mode

    int c;
    while ((c = getchar()) != '\n') { // press 'q' to quit
        if (c == 127 || c == '\b') { // backspace
            printf("[BACKSPACE]\n");
        } else {
            printf("Char: %c (%d)\n", c, c);
        }
    }

    disable_raw_mode(); // restore normal terminal
    return 0;
}

