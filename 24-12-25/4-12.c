
// Exercise 4-12. Adapt the ideas of printd to write a recursive version of itoa; that is, convert
// an integer into a string by calling a recursive routine.


#include<stdio.h>
#include<string.h>
#include <limits.h>

void my_printd(int n,char s[],int* i){
    if(n==0){
        s[*i] = '\0';
        return;
    }
    s[(*i)++] = (n%10<0) ? ((-(n%10)) + '0') : ((n%10) + '0');
    my_printd(n/10,s,i);
}

void reverse(char str[]){               // function to reverse a string inplace
    int len = strlen(str);
    for(int i=0;i<(len/2);i++){
        char t = str[i];
        str[i] = str[len-1-i];
        str[len-1-i] = t;
    }
}

void printd(int n,char s[]){           // helper function
    if(n==0){
        s[0] = '0';
        s[1] = '\0';
        return;                                                 // helper function
    }
    int i=0;
    my_printd(n,s,&i);
    if(n<0){
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
    return;
}

int main(){
    char str[100];
    printd(0,str);
    printf("%s\n",str);
    printd(INT_MIN,str);
    printf("%s\n",str);
    printd(INT_MAX,str);
    printf("%s\n",str);
}