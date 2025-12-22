#include <stdio.h>
#include <limits.h>
#include <string.h>

int atoi(char *str)
{
    int error = 0;
    int negative = 0;
    int res = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '-')
        {
            if (res == 0)
            {
                negative = 1;
            }
            else
            {
                error = 1;
                break;
            }
        }
        else if (str[i] == ' ')
        {
            continue;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            if (negative)
            {
                res *= 10;
                res -= str[i] - '0';
            }
            else
            {
                res *= 10;
                res += str[i] - '0';
            }
        }
        else
        {
            error = 1;
            break;
        }
    }
    if (error)
    {
        return -1;
    }
    return res;
}

void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0) /* record sign */
        n = -n;         /* make n positive */
    i = 0;
    do
    {                          /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0); /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void my_itoa(int n, char s[])
{
    int i, sign_negative = 0;
    if (n < 0){
        sign_negative = 1;        
    }
    i = 0;
    do
    {                          /* generate digits in reverse order */
        printf("%c \n",s[i]);
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0); /* delete it */
    if (sign_negative)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

int main()
{

    int a = INT_MIN;
    char str[100];
    my_itoa(a, str);
    printf("%s\n", str);

    return 0;
}