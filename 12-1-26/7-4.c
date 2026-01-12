// Exercise 7-4. Write a private version of scanf analogous to minprintf from the previous
// section.

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 100

void my_printf(char *fmt, ...)
{
    int ival;
    void *p_val;
    unsigned int u_ival;
    char *sval, cval;
    double fval;
    va_list ap;
    char *p;
    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }
        p++;
        int width = 0;
        while (isdigit(*p))
        {
            width = (width * 10) + (*p - '0');
            p++;
        }
        switch (*p)
        {
        case 'd':
            ival = va_arg(ap, int);
            printf("%*d", width, ival);
            break;
        case 'c':
            cval = va_arg(ap, int); // for char also int is the type
            printf("%*c", width, cval);
            break;
        case 'l':
            p++;
            if (*p == 'f')
            {
                fval = va_arg(ap, double);
                printf("%*f", width, fval);
                break;
            }
            else
            {
                break;
            }
        case 's':
            sval = va_arg(ap, char *);
            int len = strlen(sval);
            int spaces = (width > len) ? width - len : 0;
            // printf("%d,%d\n",width,spaces);
            for (int i = 0; i < spaces; i++)
            { // manual string alignment for specified width
                putchar(' ');
            }
            for (; *sval; sval++)
            {
                putchar(*sval);
            }
            break;
        case 'x': // hexadecimal
            ival = va_arg(ap, int);
            printf("%*x", width, ival);
            break;
        case 'o': // octal
            ival = va_arg(ap, int);
            printf("%*o", width, ival);
            break;
        case 'u':
            u_ival = va_arg(ap, unsigned int);
            printf("%*u", width, u_ival);
            break;
        case 'p':
            p_val = va_arg(ap, void *);
            printf("%p", p_val);
            break;
        default:
            putchar('%');
            putchar(*p);
            break;
        }
    }
    va_end(ap);
}

int my_scanf(char *fmt, ...)
{
    int *ival;
    double *fval;
    char *sval, *cval;
    char *p = fmt;

    int cnt = 0;

    va_list ap;
    va_start(ap, fmt);

    for (p; *p != '\0'; p++)
    {
        if (*p == '%')
        {
            p++;
            switch (*p)
            {
            case 'd':
                int dnum;
                ival = va_arg(ap, int *);
                scanf("%d", &dnum);
                *ival = dnum;
                cnt++;
                break;
            case 'l':
                p++;
                if (*p == 'f')
                {
                    double fnum;
                    fval = va_arg(ap, double *);
                    scanf("%lf", &fnum);
                    *fval = fnum;
                    cnt++;
                    break;
                }
                else
                {
                    break;
                }
            case 'c':
                char c;
                cval = va_arg(ap, char *);
                scanf("%c", &c);
                *cval = c;
                cnt++;
                break;
            case 's':
                char s[MAX_LINE_LENGTH];
                sval = va_arg(ap, char *);
                scanf("%s", s);
                for (int i = 0; s[i] != '\0'; i++)
                {
                    *sval = s[i];
                    sval++;
                }
                *sval = '\0';
                cnt++;
                break;
            }
        }
    }
    va_end(ap);
    return cnt;
}

int main()
{

    char a, b[MAX_LINE_LENGTH];
    int c;
    double d;
    printf("%d\n", my_scanf("%c %s %d %lf", &a, b, &c, &d));

    my_printf("%c\n%s\n%d\n%lf\n",a,b,c,d);

    // input : z nirmal 3432 121.3242

    return 0;
}