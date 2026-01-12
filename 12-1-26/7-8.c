#include <stdio.h>
#define LINES_PER_PAGE 3
#define MAX_LINE 100

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            FILE *fp = fopen(argv[i], "r");
            if (!fp)
            {
                fprintf(stderr, "Cannot open %s\n", argv[i]);
                continue;
            }
            char buffer[MAX_LINE];
            int line = 1, page = 1;
            printf("FILE : %s\tPage : %d\n", argv[i], page);
            printf("..................................\n");
            while ((fgets(buffer, MAX_LINE, fp)) != NULL)
            {
                printf("%d. %s", line++, buffer);
                if ((line-1) % LINES_PER_PAGE == 0)
                {
                    printf("\n");
                    page++;
                    printf("FILE : %s\tPage : %d\n", argv[i], page);
                    printf("..................................\n");
                }
            }
            printf("\n");
            printf(".......................................\n");
            fclose(fp);
        }
    }
    return 0;
}