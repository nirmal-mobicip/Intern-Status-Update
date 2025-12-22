// Exercise 3-2. Write a function escape(s,t) that converts characters like newline and tab
// into visible escape sequences like \n and \t as it copies the string t to s. Use a switch. Write
// a function for the other direction as well, converting escape sequences into the real characters.

#include<stdio.h>

void escape(char* s,char* t){
    int ptr=0;
    for(int i=0;s[i]!='\0';i++){
        switch(s[i]){
            case '\n':
                t[ptr++] = '\\';
                t[ptr++] = 'n';
                break;                              // demonstration of switch case
            case '\t':
                t[ptr++] = '\\';
                t[ptr++] = 't';
                break;
            default:
                t[ptr++] = s[i];
                break;
        }
    }
    t[ptr] = '\0';                                 // added null terminator
}

int main(){
    char s[100];
    int ptr=0;
    char ch;
    while((ch=getchar())!=EOF){
        s[ptr++] = ch;
    }
    s[ptr] = '\0';                                  // added null terminator

    // char s[100] = "hello\nguys\thowareyou";

    char t[100];
    escape(s,t);
    printf("%s\n",t);
    return 0;
}