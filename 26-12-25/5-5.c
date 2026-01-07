// Exercise 5-5. Write versions of the library functions strncpy, strncat, and strncmp, which
// operate on at most the first n characters of their argument strings. For example,
// strncpy(s,t,n) copies at most n characters of t to s. Full descriptions are in Appendix B.

#include<stdio.h>
#include<string.h>

void my_strncpy(char* dest, char* src, int n){
    char *s = src, *d = dest, i=0;
    while(i<n && *s != '\0'){
        *d++ = *s++;
        i++;
    }
    while(i<n){
        *d++ = '\0';
        i++;
    }
}

void my_strncat(char* s1, char* s2, int n){
    char *ptr = s1; 
    while(*ptr != '\0'){
        ptr++;
    }
    int i=0;
    while(i<n && *s2 != '\0'){
        *ptr++ = *s2++;
        i++;
    }
    *ptr = '\0';
}

int my_strncmp(char* s1,char* s2, int n){
    if(n==0) return 0;
    char* p1 = s1, *p2 = s2;
    int i = 0;
    while(i<n && *p1 == *p2){
        printf("i = %d l = %c r = %c\n",i,*p1,*p2);
        if(*p1=='\0'){
            return 0;
        }
        p1++;
        p2++;
        i++;
    }
    return (i==n) ? 0 : (unsigned char)*p1 - (unsigned char)*p2;
}

int main(){

    char s1[] = "heloword";
    char s2[] = "heloworld";
    printf("%d\n",my_strncmp(s1,s2,7));
    return 0;
}