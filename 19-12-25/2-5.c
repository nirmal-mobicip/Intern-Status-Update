// Exercise 2-5. Write the function any(s1,s2), which returns the first location in a string s1
// where any character from the string s2 occurs, or -1 if s1 contains no characters from s2.
// (The standard library function strpbrk does the same job but returns a pointer to the
// location.)

#include<stdio.h>

int my_strpbrk(char* str1, char* str2){
    char set[256] = {0};
    for(int i=0;str2[i]!='\0';i++){
        set[(unsigned char)str2[i]] = 1;                            // store the characters in set
    }
    for(int i=0;str1[i]!='\0';i++){
        if(set[(unsigned char)str1[i]]==1){                         // checks and stops if any character in s1 is found in set and returns the index
            return i;
        }
    }
    return -1;
}

int main(){
    char str[] = "abcdef";
    char breakset[] = "fed";
    printf("%d\n",my_strpbrk(str,breakset));
    return 0;
}