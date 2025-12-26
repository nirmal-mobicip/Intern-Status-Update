// Exercise 5-2. Write getfloat, the floating-point analog of getint. What type does
// getfloat return as its function value?


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

int getfloat(float *pn)
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
    for (*pn = 0; isdigit(c) || c==' '; c = getch()){
        if(c==' ') continue;
        *pn = 10 * (*pn) + (c - '0');
    }
    if(c!=EOF && c=='.'){
        c = getch();
        float power=1;
        for (*pn;isdigit(c) || c==' '; c = getch()){
            if(c==' ') continue;
            *pn = 10 * (*pn) + (c - '0');
            power*=10;
        }
        *pn /= power;
    }
    
    *pn *= sign;

    if (c != EOF)
        ungetch(c);

    return c;
}


int main()
{
    float a;
    getfloat(&a)!=0 ? printf("%f\n",a) : printf("Not a number\n");
    return 0;
}