#include <stdio.h>
#include <string.h>

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

void itob(int n, char str[], int base)
{
    if (base >= 2 && base <= 16)
    {
        int ptr = 0;
        if (n == 0)
        {
            str[ptr++] = '0';
        }
        int sign = n;
        if(n<0){
            n = -n;
        }

        while (n > 0)
        {
            int c = n % base;
            if (c > 9)
            {
                str[ptr] = (c-10) + 'A';
            }
            else
            {
                str[ptr] = c + '0';
            }
            ptr++;
            n /= base;
        }
        if(sign<0){
            str[ptr++] = '-';
        }
        str[ptr] = '\0';
        reverse(str);
    }
}

int main()
{

    int a = -120;
    char str[100];
    itob(a, str, 15);
    printf("%s\n", str);

    return 0;
}