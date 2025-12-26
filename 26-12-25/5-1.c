// Exercise 5-1. As written, getint treats a + or - not followed by a digit as a valid
// representation of zero. Fix it to push such a character back on the input.


#include <stdio.h>
#define MAX 100

int buf[MAX];
int bufp = 0;

int getch(){
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)
{
    if (bufp >= MAX){
        printf("Too many characters\n");
    }
    else{
        buf[bufp++] = c;
    }
}

#include <ctype.h>

int getint(int *pn)
{
    int c, sign;

    /* skip white space */
    while (isspace(c = getch()))
        ;

    if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
        ungetch(c);      /* it is not a number */
        return 0;
    }

    sign = (c == '-') ? -1 : 1;

    if (c == '+' || c == '-'){
        int next = getch();
        if(!isdigit(next)){
            ungetch(next);
            ungetch(c);
            return 0;
        }
        c = next;
    }
    for (*pn = 0; isdigit(c); c = getch())
        *pn = 10 * (*pn) + (c - '0');

    *pn *= sign;

    if (c != EOF)
        ungetch(c);

    return c;
}


int main()
{
    int *a;
    getint(a);
    printf("%d\n",*a);
    return 0;
}