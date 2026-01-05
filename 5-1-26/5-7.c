// Exercise 5-7. Rewrite readlines to store lines in an array supplied by main, rather than
// calling alloc to maintain storage. How much faster is the program?

#include <stdio.h>
#include <string.h>

#define MAXLINES 5000
#define MAXLEN 1000
#define SIZE 10000


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


void swap(char** s1,char** s2){
    char* temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}

void sort(char *array[], int nlines)
{
    for (int j = 0; j < nlines; j++)
    {
        for (int i = 0; i < nlines - 1; i++)
        {
            if (strcmp(array[i], array[i + 1]) > 0)
            {
                swap(&array[i], &array[i + 1]);
            }
        }
    }
}

int readlines(char storage[], char* lineptr[], int maxlines, int maxsize)
{
    int nlines = 0;
    char line[MAXLEN];
    int len,pos=0;

    while (nlines <= maxlines && (len = my_getline(line)) > 0)
    {
        strcpy(&storage[pos],line);
        lineptr[nlines++] = &storage[pos];
        pos+=len+1;
        if(pos>=maxsize){
            break;
        }
    }
    return nlines;
}

int main()
{   
    char storage[SIZE];
    char* lineptr[MAXLINES];
    
    int nlines = readlines(storage,lineptr, MAXLINES,SIZE);
    
    sort(lineptr, nlines);
    
    for (int i = 0; i < nlines; i++)
    {
        printf("%s\n", lineptr[i]);
    }
    
    return 0;
}