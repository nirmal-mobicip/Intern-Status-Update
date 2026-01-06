// Exercise 5-15. Add the option -f to fold upper and lower case together, so that case
// distinctions are not made during sorting; for example, a and A compare equal.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

    while (nlines < maxlines && (len = my_getline(line)) > 0)
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

void swap(char **s1, char **s2)
{
    char *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

int str_case_cmp(void *s1, void *s2)
{
    char *p1 = (char *)s1;
    char *p2 = (char *)s2;

    for (; *p1 && *p2; p1++, p2++)
    {
        int c1 = tolower((unsigned char)*p1);
        int c2 = tolower((unsigned char)*p2);

        if (c1 != c2)
            return c1 - c2;
    }

    return tolower((unsigned char)*p1) -
           tolower((unsigned char)*p2);
}


void sort(char *lineptr[], int nlines, int (*compare)(void *, void *), int reverse)
{
    for (int j = 0; j < nlines; j++)
    {
        int swapped = 0;
        for (int i = 0; i < nlines - 1 - j; i++)
        {   
            int cond = compare(lineptr[i],lineptr[i + 1]);

            if (reverse)
                cond = -cond;

            if (cond > 0)
            {
                swap(&lineptr[i], &lineptr[i + 1]);
                swapped=1;
            }
        }
        if (!swapped) break;
    }
}

int numcmp(void *s1, void *s2)
{
    double v1, v2;
    v1 = atof((char *)s1);
    v2 = atof((char *)s2);
    if (v1 < v2)
        return -1;
    else if (v1 > v2)
        return 1;
    else
        return 0;
}

int str_cmp(void *s1, void *s2)
{
    return strcmp((char *)s1, (char *)s2);
}

int main(int argc, char *argv[])
{

    int rev = 0, numeric = 0, not_case_sensitive = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-n") == 0)
            numeric = 1;
        else if (strcmp(argv[i], "-r") == 0)
            rev = 1;
        else if (strcmp(argv[i],"-f")==0){
            not_case_sensitive = 1;
        }
    }

    int nlines = readlines(lineptr, MAXLINES);

    sort(lineptr, nlines, numeric ? numcmp : (not_case_sensitive? str_case_cmp : str_cmp), rev);

    for (int i = 0; i < nlines; i++)
    {
        printf("%s\n", lineptr[i]);
    }

    return 0;
}