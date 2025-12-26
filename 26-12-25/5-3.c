// Exercise 5-3. Write a pointer version of the function strcat that we showed in Chapter 2:
// strcat(s,t) copies the string t to the end of s.

#include<stdio.h>
#include<string.h>

void my_strcat(char* s1, char* s2){
    char *ptr = s1;
    while(*ptr != '\0')
        ptr++;

    while(*s2 != '\0')
        *ptr++ = *s2++;
    *ptr = '\0';
}

int main(){

    char s1[] = "nirmal";
    char s2[] = "mobicip";
    my_strcat(s1,s2);
    printf("%s\n",s1);

    return 0;
}