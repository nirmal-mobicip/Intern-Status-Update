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
            // printf("n mod 10 = %d s[ptr] = %c\n",(n%10)*-1,s[ptr]);
            ptr++;
            n/=10;
       }
       s[ptr++] = '-';
    }else{
        s[ptr++] = '0';
    }
    for(int i=ptr;i<w;i++){
        s[ptr++] = ' ';
    }
    s[ptr] = '\0';
    reverse(s);
}

int main(){

    int a = 1;
    char str[100];
    my_itoa(a,str,0);
    printf("*%s\n",str);

    return 0;
}