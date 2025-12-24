// Exercise 4-11. Modify getop so that it doesn't need to use ungetch. Hint: use an internal
// static variable.

#include <stdio.h>
#include <ctype.h>
#define MAX 100
#define NUMBER '0'


int buf[MAX];
int bufp = 0;

int getch()
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)
{
    if (bufp >= MAX)
    {
        printf("Too many characters\n");
    }
    else
    {
        buf[bufp++] = c;
    }
}

int getop(char s[])
{
    static int buf = EOF;                   // start static variable with EOF

    int c;
    if(buf!=EOF){                           // if not empty return to input and not call getchar
        c = buf;
        printf("from buf : %c\n",buf);
        buf = EOF;
    }else{                                  // if empty call getchar()
        c = getchar();
    }
    while (c == ' ' || c == '\t'){
        c = getchar();
    }
    s[0] = c;
    s[1] = '\0';
    if (!isdigit(c) && c != '.')            // checks if it is non digit and non dot then its a operator
    {
        return c;
    }
    int i = 0;
    if (isdigit(c))                         // checks if its a digit
    {
        while (isdigit(s[++i] = c = getchar()))     // get digit
            ;
    }
    if(c=='.'){                             // if . found
        while (isdigit(s[++i] = c = getchar()))     // get fraction part
            ;
    }
    s[i] = '\0';
    if(c!=EOF){
        // ungetch(c);
        buf = c;                            // instead to store in buf of ungetch() storing in to static variable
    }
    return NUMBER;
}

int main()
{
    char s[10],i=0;
    while(getop(s)!=EOF){
        printf("%d - %s\n",i++,s);
    }
    return 0;
}