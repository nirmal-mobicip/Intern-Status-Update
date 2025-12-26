
// Exercise 3-6. Write a version of itoa that accepts three arguments instead of two. The third
// argument is a minimum field width; the converted number must be padded with blanks on the
// left if necessary to make it wide enough.


#include<stdio.h>
#include<string.h>

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

void my_itoa(int n, char s[],int w)
{
    int ptr = 0;
    if(n>0){
       while(n>0){
            s[ptr++] = (n%10 + '0');
            n/=10;
       }
       
    }else if(n<0){
        while(n<0){
            s[ptr] = (((n%10)*-1) + '0');
            ptr++;
            n/=10;
       }
       s[ptr++] = '-';
    }else{
        s[ptr++] = '0';
    }
    for(int i=ptr;i<w;i++){                                                 // adds minimum no of spaces if necessary at the end and reverses   
        s[ptr++] = ' ';                                                     // so the space comes at begining
    }
    s[ptr] = '\0';
    reverse(s);
}

int main(){

    int a = 1;
    char str[100];
    my_itoa(a,str,3);
    printf("*%s\n",str);

    return 0;
}