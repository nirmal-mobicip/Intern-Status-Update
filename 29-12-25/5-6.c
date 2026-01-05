#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define NUMBER '0'

#define BUFSIZE 100

char buf[BUFSIZE];   
int bufp = 0;        

int getch(void)      
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c)  
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}


void my_getline(char* a){
    char* p = a,ch;
    while((ch=getchar())!='\n' && ch!=EOF){
        *p++ = ch;
    }
    *p = '\0';
}

void my_reverse(char* a){
    char* l = a;
    char* r = a+strlen(a)-1;
    while(l<r){
        char temp = *l;
        *l++ = *r;
        *r-- = temp;
    }
}
int strindex(char *s, char *t) {
    char *p, *q, *r;

    for (p = s; *p != '\0'; p++) {
        q = p;
        r = t;

        while (*r != '\0' && *q == *r) {
            q++;
            r++;
        }

        if (r != t && *r == '\0')
            return p - s;   // index found
    }
    return -1;
}

void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0) {   
        n = -n;            
    }

    char* p = s;
    do {
        *p++ = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        *p++ = '-';
    }

    *p = '\0';
    my_reverse(s);
}

int atoi(char s[])
{
    int i, n, sign;
    char* p = s;

    while(isspace(*p)){
        *p++;
    }

    sign = (*p == '-') ? -1 : 1;

    if (*p == '+' || *p == '-')
        *p++;

    for (n = 0; isdigit(*p); p++)
        n = 10 * n + (*p - '0');

    return sign * n;
}

int getop(char s[])
{
    int c;
    char *p = s;

    while ((*p = c = getch()) == ' ' || c == '\t')
        ;

    *(p + 1) = '\0';

    if (!isdigit((unsigned char)c) && c != '.')
        return c;

    if (isdigit((unsigned char)c)) {
        while (isdigit((unsigned char)(*(++p) = c = getch())))
            ;
    }

    if (c == '.') {
        while (isdigit((unsigned char)(*(++p) = c = getch())))
            ;
    }

    *p = '\0';

    if (c != EOF)
        ungetch(c);

    return NUMBER;
}






int main(){
    char b[100];

    while(getop(b)){
        printf("%s\n",b);
    }
    return 0;
}
