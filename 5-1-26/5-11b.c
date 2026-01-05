// Exercise 5-11. Modify the program entab and detab (written as exercises in Chapter 1) to
// accept a list of tab stops as arguments. Use the default tab settings if there are no arguments.

#include <stdio.h>

#define TAB_WIDTH 8

int main() {
    int c, pos = 1, nb = 0, nt = 0;

    for (pos = 1; (c = getchar()) != EOF; ++pos) {
        if (c == ' ') {
            if ((pos % TAB_WIDTH) != 0) {
                ++nb; 
            } else {
                nb = 0; 
                ++nt;   
            }
        } else {
            for (; nt > 0; --nt) putchar('\t');
            
            if (c == '\t') {
                nb = 0;
            } else {
                for (; nb > 0; --nb) putchar(' ');
            }
            
            putchar(c);
            
            if (c == '\n') {
                pos = 0;
            } else if (c == '\t') {
                pos = pos + (TAB_WIDTH - (pos - 1) % TAB_WIDTH) - 1;
            }
        }
    }
    return 0;
}
