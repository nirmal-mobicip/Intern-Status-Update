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
    return NULL;
}

char *lineptr[MAXLINES];

// getline 
int my_getline(char *s)
{
    int c, i = 0;
    while (i < MAXLEN - 1 && (c = getchar()) != EOF && c != '\n')
        s[i++] = c;
    s[i] = '\0';
    return i;
}

// read lines 
int readlines(char *lineptr[], int maxlines)
{
    int nlines = 0, len;
    char line[MAXLEN];

    while (nlines < maxlines && (len = my_getline(line)) > 0)
    {
        char *p = alloc(len + 1);
        if (!p)
            return -1;
        strcpy(p, line);
        lineptr[nlines++] = p;
    }
    return nlines;
}

// swap pointers 
void swap(char **a, char **b)
{
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}

// numeric compare 
int numcmp(void *s1, void *s2)
{
    double v1 = atof((char *)s1);
    double v2 = atof((char *)s2);
    return (v1 > v2) - (v1 < v2);
}

// normal string compare 
int str_cmp(void *s1, void *s2)
{
    return strcmp((char *)s1, (char *)s2);
}

// case-folded string compare 
int str_case_cmp(void *s1, void *s2)
{
    char *p1 = s1, *p2 = s2;
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

// directory order compare
int dir_cmp(void *s1, void *s2)
{
    char *p1 = s1, *p2 = s2;

    while (*p1 || *p2)
    {
        while (*p1 && !isalnum((unsigned char)*p1) && *p1 != ' ')
            p1++;
        while (*p2 && !isalnum((unsigned char)*p2) && *p2 != ' ')
            p2++;

        if (*p1 == '\0' || *p2 == '\0')
            break;

        if (*p1 != *p2)
            return (unsigned char)*p1 - (unsigned char)*p2;

        p1++;
        p2++;
    }
    return (unsigned char)*p1 - (unsigned char)*p2;
}

// directory + case-fold compare 
int dir_case_cmp(void *s1, void *s2)
{
    char *p1 = s1, *p2 = s2;

    while (*p1 || *p2)
    {
        while (*p1 && !isalnum((unsigned char)*p1) && *p1 != ' ')
            p1++;
        while (*p2 && !isalnum((unsigned char)*p2) && *p2 != ' ')
            p2++;

        if (*p1 == '\0' || *p2 == '\0')
            break;

        int c1 = tolower((unsigned char)*p1);
        int c2 = tolower((unsigned char)*p2);

        if (c1 != c2)
            return c1 - c2;

        p1++;
        p2++;
    }
    return tolower((unsigned char)*p1) -
           tolower((unsigned char)*p2);
}

// bubble sort
void sort(char *v[], int n,
          int (*cmp)(void *, void *),
          int reverse)
{
    for (int i = 0; i < n - 1; i++)
    {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++)
        {
            int r = cmp(v[j], v[j + 1]);
            if (reverse)
                r = -r;
            if (r > 0)
            {
                swap(&v[j], &v[j+1]);
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

int main(int argc, char *argv[])
{
    int numeric = 0, reverse = 0, fold = 0, directory = 0;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-n"))
            numeric = 1;
        else if (!strcmp(argv[i], "-r"))
            reverse = 1;
        else if (!strcmp(argv[i], "-f"))
            fold = 1;
        else if (!strcmp(argv[i], "-d"))
            directory = 1;
    }

    int nlines = readlines(lineptr, MAXLINES);
    if (nlines < 0)
    {
        printf("input too big\n");
        return 1;
    }

    int (*cmp)(void *, void *);

    if (numeric)
        cmp = numcmp;
    else if (directory && fold)
        cmp = dir_case_cmp;
    else if (directory)
        cmp = dir_cmp;
    else if (fold)
        cmp = str_case_cmp;
    else
        cmp = str_cmp;

    sort(lineptr, nlines, cmp, reverse);

    for (int i = 0; i < nlines; i++)
        printf("%s\n", lineptr[i]);

    return 0;
}
