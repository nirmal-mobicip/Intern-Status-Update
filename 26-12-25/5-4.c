// Exercise 5-4. Write the function strend(s,t), which returns 1 if the string t occurs at the
// end of the string s, and zero otherwise.


#include<stdio.h>
#include<string.h>

int my_strend(char* s1, char* s2){
    int l1 = strlen(s1), l2 = strlen(s2);
    if(l1<l2){
        return 0;
    }
    char *p1 = s1+l1-1,*p2 = s2+l2-1;
    
    while(p2 != s2-1 && p1 != s1-1){
        if(*p2-- != *p1--){
            return 0;
        }
    }
    return 1;
}

int main(){

    char s1[] = "hello world";
    char s2[] = "rld";
    printf("%d\n",my_strend(s1,s2));

    return 0;
}