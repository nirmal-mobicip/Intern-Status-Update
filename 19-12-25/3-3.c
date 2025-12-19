// Exercise 3-3. Write a function expand(s1,s2) that expands shorthand notations like a-z in
// the string s1 into the equivalent complete list abc...xyz in s2. Allow for letters of either
// case and digits, and be prepared to handle cases like a-b-c and a-z0-9 and -a-z. Arrange
// that a leading or trailing - is taken literally.

#include<stdio.h>
#include<string.h>

void expand(char* str1,char* str2){
    int len = strlen(str1),ptr = 0;
    for(int i=0;i<len;i++){
        if(str1[i]=='-'){
            if(i-1>=0 && i+1<len){                                                          // to check condition eg (G-C is wrong)
                if(str1[i-1]<str1[i+1] && (str1[i+1]-str1[i-1])<=25){                       // to check condition eg (A-d is wrong)
                    for(int j = str1[i-1];j<=str1[i+1];j++){
                        if(ptr==0){
                            str2[ptr++] = j;
                        }else if(ptr!=0 && str2[ptr-1]!=j){                                 // to overcome printing of same characters twice eg for a-c-e -> abc cde == abccde
                            // printf("ptr = %d prev = %c\n",ptr,str2[ptr-1]);              // actual answer will be abcde
                            str2[ptr++] = j;
                        }
                    }
                }else{
                    str2[ptr++] = '-';
                }
            }else{
                str2[ptr++] = '-';
            }
        }
    }
    str2[ptr] = '\0';
}

int main(){

    char str1[] = "a-f-g0-8";
    char str2[1000];
    expand(str1,str2);
    for(int i=0;str2[i]!='\0';i++){
        printf("%c",str2[i]);
    }
    printf("\n");
    return 0;
}