
// Exercise 4-1. Write the function strindex(s,t) which returns the position of the rightmost
// occurrence of t in s, or -1 if there is none.


#include<stdio.h>
#include<string.h>

int strindex(char s[], char t[]){
    int lens = strlen(s);
    int lent = strlen(t);
    if(lent==0) return 0;
    if(lent<=lens){
        for(int i=lens-1;i>=0;i--){
            if(t[lent-1]==s[i]){                                        // if last element of t is found in s
                int f=1,j,k;                                            // traverses to find the entire string exists
                for(j=i,k=lent-1;j>=0 && j>=i-lent+1;j--,k--){          
                    if(s[j]!=t[k]){                             
                        f=0;
                        break;
                    }
                }
                if(f==1){
                    return j+1;
                }
            }
        }
        return -1;
    }else{
        return -1;
    }
}

int main(){
    char s[] = "sdfgfsdjf";
    char t[] = "f";
    printf("%d\n",strindex(s,t));
    return 0;
}