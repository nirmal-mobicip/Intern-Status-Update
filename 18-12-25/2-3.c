/*Exercise 2-3. Write a function htoi(s), which converts a string of hexadecimal digits
(including an optional 0x or 0X) into its equivalent integer value. The allowable digits are 0
through 9, a through f, and A through F.*/

#include<stdio.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>


int htoi(char* str){
    int len = strlen(str);
    if(len>2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')){               // checks for trailing '0X' or '0x'
        int res = 0;
        int val = 1;
        for(int i=len-1;i>=2;i--){                                          // scans from end
            if(isdigit(str[i])!=0){
                res+=(val*(str[i]-'0'));
            }else if(toupper(str[i])>='A' && toupper(str[i])<='F'){
                    res += (val* (toupper(str[i])-'A'+10));
            }else{
                res = -1;
                break;
            }
            val*=16;
        }
        return res;                                                                  //returns result(integer)
    }else{
        return -1;                                                                   // if does'nt begin with either '0x' or '0X' returns -1(error)
    }
}

int main(){
    printf("%d\n",htoi("0x7FFFFFFF"));
    printf("%d\n",INT_MAX);
    return 0;
}