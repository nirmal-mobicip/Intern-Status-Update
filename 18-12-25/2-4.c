// Exercise 2-4. Write an alternative version of squeeze(s1,s2) that deletes each character in
// s1 that matches any character in the string s2.

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void squeeze(char* s1, char* s2){
    char set[256] = {0};                                        //created a set to store the unique values and make the lookup to O(1)
    for(int i=0;s2[i]!='\0';i++){
        set[(unsigned char)s2[i]] = 1;              //store the s2 into the set
    }                                   
    int ptr=0;
    for(int i=0;s1[i]!='\0';i++){
        if(set[(unsigned char)s1[i]]==0){           // check each character in s1 that is inside the set
            s1[ptr++] = s1[i];                      // if not found then retain the character and if found
        }                                           // skip it
        
    }
    s1[ptr] = '\0';
}

int main(){

    char str1[] = "";
    char str2[] = "";
    squeeze(str1,str2);
    printf("%s\n",str1);
    return 0;
}