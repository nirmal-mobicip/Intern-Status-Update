/*Exercise 1-23. Write a program to remove all comments from a C program. Don't forget to
handle quoted strings and character constants properly. C comments don't nest.*/


#include <stdio.h>
#include <limits.h>

#define TAB 4

int getLine(char line[])
{
    int len = 0;
    char ch = getchar();
    while (ch != EOF && ch != '\n')
    {
        line[len++] = ch;
        ch = getchar();
    }
    return len;
}

void printLine(char line[], int len)
{
    for (int i = 0; i < len; i++)
    {
        putchar(line[i]);
    }
    printf("\n");
}

void removeComments(char line[], int len)
{
    if (len > 2)
    {
        int p1 = 0, p2 = 1;
        while (p2 < len)
        {
            if (line[p1] == '/' && line[p2] == '/')
            {
                break;
            }else{
                putchar(line[p1]);
            }
            p1++;
            p2++;
        }
        printf("\n");
    }
}

int main()
{

    char current_line[100];
    int len = 1;
    while (len > 0)
    {
        len = getLine(current_line);
        removeComments(current_line, len);
    }
    return 0;
}