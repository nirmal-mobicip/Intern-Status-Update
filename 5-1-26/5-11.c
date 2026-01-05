#include <stdio.h>
#include <stdlib.h>

#define TABSPACE 4

int main(int argc, char* argv[])
{
    int pos = 0;
    char ch;
    printf("%d\n",atoi(argv[argc-1]));
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if (ch == '\t')
        {
            if(atoi(argv[argc-1])<pos){                             // if pos greater than largest argument
                int spaces = TABSPACE - (pos%TABSPACE);             // do the default
                while(spaces--){
                    putchar(' ');
                    pos++;
                }
            }else{
                int arg;
                for(int i=0;i<argc;i++){                            // finds the nearest greater arg
                    if((arg = atoi(argv[i]))>pos){
                        break;
                    }
                }
                int spaces = arg-pos;
                while(spaces--){                                    // leaves that no of spaces as required
                    putchar(' ');
                    pos++;
                }
            }
        }
        else
        {
            putchar(ch);
            if (ch == '\n')
            {
                pos = 0;
            }
            else
            {
                pos++;
            }
        }
    }
    return 0;
}