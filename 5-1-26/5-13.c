// Exercise 5-13. Write the program tail, which prints the last n lines of its input. By default, n
// is set to 10, let us say, but it can be changed by an optional argument so that
// tail -n
// prints the last n lines. The program should behave rationally no matter how unreasonable the
// input or the value of n. Write the program so it makes the best use of available storage; lines
// should be stored as in the sorting program of Section 5.6, not in a two-dimensional array of
// fixed size.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINES 5000
#define MAXLEN 1000
#define SIZE 10000

static char memory[SIZE];
static char *freeptr = memory;

char *alloc(int len)
{
    if (freeptr + len <= memory + SIZE)
    {
        freeptr += len;
        return freeptr - len;
    }
    else
    {
        return 0;
    }
}
int my_getline(char *str)
{
    char ch;
    int i = 0;
    while (i < MAXLEN-1 && (ch = getchar()) != '\n' && ch != EOF)
    {
        str[i++] = ch;
    }
    str[i] = '\0';
    return i;
}
int readlines(char *lineptr[], int maxlines, int* end)
{
    int nl = 0;
    char line[MAXLEN];
    int len;

    while ((len = my_getline(line)) > 0)
    {
        char *p = alloc(len + 1); // space for null terminator
        if (p == NULL){
            return -1;
        }
        strcpy(p, line);
        lineptr[nl%maxlines] = p;
        *end = nl%maxlines;
        nl++;
    }
    return nl;
}
int main(int argc, char* argv[])
{
    int nl = 10;
    int end;
    if(argc>1){
        nl = atoi(argv[1]);
    }
    char *lineptr[nl];
    int total = readlines(lineptr, nl, &end);
    int bufsize = nl>total ? total :nl ;
    int start = total>nl ? total%nl: 0;
    for(int i=0;i<bufsize;i++){
        printf("%s\n",lineptr[(start+i)%nl]);
    }
    return 0;
}