// Exercise 7-6. Write a program to compare two files, printing the first line where they differ.

#include <stdio.h>
#include <string.h>
#include<stdlib.h>

#define MAX_LINE_SIZE 100

void compare_file(FILE *f1, FILE *f2)
{
    int line = 1;
    char b1[MAX_LINE_SIZE], b2[MAX_LINE_SIZE];
    while (1)
    {
        char *l1 = fgets(b1, MAX_LINE_SIZE, f1);
        char *l2 = fgets(b2, MAX_LINE_SIZE, f2);
        if (l1 == NULL && l2 == NULL)
        {
            printf("Files are Identical\n");
            return;
        }
        else if (l1 == NULL && l2 != NULL)
        {
            printf("Line %d : %s",line, b2);
            return;
        }
        else if (l1 != NULL && l2 == NULL)
        {
            printf("Line %d : %s",line, b1);
            return;
        }
        else
        {
            if (strcmp(b1, b2) != 0)
            {
                printf("File 1 : Line %d : %s",line, b1);
                putchar('\n');
                printf("File 2 : Line %d : %s",line, b2);
                putchar('\n');
                printf("File Mismatch\n");
                return;
            }
            line++;
        }
    }
}

int main(int argc, char *argv[])
{

    if (argc == 3)
    {
        FILE *f1 = fopen(argv[1], "r");
        FILE *f2 = fopen(argv[2], "r");
        if(!f1 || !f2){
            fprintf(stderr,"Error opening file\n");
            exit(1);
        }
        compare_file(f1, f2);
        fclose(f1);
        fclose(f2);
    }

    return 0;
}