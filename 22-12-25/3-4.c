
// Exercise 3-4. In a two's complement number representation, our version of itoa does not
// handle the largest negative number, that is, the value of n equal to -(2wordsize-1). Explain why
// not. Modify it to print that value correctly, regardless of the machine on which it runs.

#include <stdio.h>
#include <limits.h>
#include <string.h>

// int atoi(char *str)
// {
//     int error = 0;
//     int negative = 0;
//     int res = 0;
//     for (int i = 0; str[i] != '\0'; i++)
//     {
//         if (str[i] == '-')
//         {
//             if (res == 0)
//             {
//                 negative = 1;
//             }
//             else
//             {
//                 error = 1;
//                 break;
//             }
//         }
//         else if (str[i] == ' ')
//         {
//             continue;
//         }
//         else if (str[i] >= '0' && str[i] <= '9')
//         {
//             if (negative)
//             {
//                 res *= 10;
//                 res -= str[i] - '0';
//             }
//             else
//             {
//                 res *= 10;
//                 res += str[i] - '0';
//             }
//         }
//         else
//         {
//             error = 1;
//             break;
//         }
//     }
//     if (error)
//     {
//         return -1;
//     }
//     return res;
// }

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
        n = -n;         /* make n positive */                                         // range of int is -2,147,483,648	to 2,147,483,647
    i = 0;                                                                            // so if n is min value and converted to positive int cannot hold it
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
    int ptr = 0;
    if(n>0){
       while(n>0){
            s[ptr++] = (n%10 + '0');                                // so to overcome that problem keep the number as it is
            n/=10;                                                          
       }
       s[ptr] = '\0';
    }else if(n<0){
        while(n<0){
            s[ptr] = (((n%10)*-1) + '0');
            ptr++;
            n/=10;
       }
       s[ptr++] = '-';
       s[ptr] = '\0';
    }else{
        s[ptr++] = '0';
        s[ptr] = '\0';
    }
    reverse(s);
}

int main()
{

    int a = 0;
    char str[100];
    my_itoa(a, str);

    printf("%s\n", str);

    return 0;
}