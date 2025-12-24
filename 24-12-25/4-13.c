
// Exercise 4-13. Write a recursive version of the function reverse(s), which reverses the
// string s in place.


#include<stdio.h>
#include<string.h>

void my_rev(char s[],int l,int r){
    if(l<r){
        char t = s[l];
        s[l] = s[r];
        s[r] = t;
        my_rev(s,l+1,r-1);
    }
    return;
}

void reverse(char s[]){                 // helper function
    my_rev(s,0,strlen(s)-1);
}

int main(){
    char str[] = "1234";
    reverse(str);
    printf("%s\n",str);
    return 0;
}