#include <stdio.h>
#include <string.h>

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

char *lineptr[MAXLINES];

int my_getline(char *str)
{
    char ch;
    int i = 0;
    while (i < MAXLEN && (ch = getchar()) != '\n' && ch != EOF)
    {
        str[i++] = ch;
    }
    str[i] = '\0';
    return i;
}

int readlines(char *lineptr[], int maxlines)
{
    int nlines = 0;
    char line[MAXLEN];
    int len;

    while (nlines <= maxlines && (len = my_getline(line)) > 0)
    {
        char *p = alloc(len + 1); // space for null terminator
        if (p == NULL)
        {
            return -1;
        }
        strcpy(p, line);
        lineptr[nlines++] = p;
    }
    return nlines;
}

void swap(char** s1,char** s2){
    char* temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

void sort(char *lineptr[], int nlines)
{
    for (int j = 0; j < nlines; j++)
    {
        for (int i = 0; i < nlines - 1; i++)
        {
            if (strcmp(lineptr[i], lineptr[i + 1]) > 0)
            {
                swap(&lineptr[i], &lineptr[i + 1]);
            }
        }
    }
}

int main()
{

    int nlines = readlines(lineptr, MAXLINES);

    sort(lineptr, nlines);

    for (int i = 0; i < nlines; i++)
    {
        printf("%s\n", lineptr[i]);
    }

    return 0;
}