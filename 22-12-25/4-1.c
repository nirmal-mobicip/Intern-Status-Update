#include<stdio.h>
#include<string.h>

int strindex(char s[], char t[]){
    int lens = strlen(s);
    int lent = strlen(t);
    if(lent==0) return 0;
    if(lent<=lens){
        for(int i=lens-1;i>=0;i--){
            if(t[lent-1]==s[i]){
                int f=1,j,k;
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
    char s[] = "sdfg";
    char t[] = "f";
    printf("%d\n",strindex(s,t));
    return 0;
}