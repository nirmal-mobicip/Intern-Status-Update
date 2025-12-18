/*Exercise 2-3. Write a function htoi(s), which converts a string of hexadecimal digits
(including an optional 0x or 0X) into its equivalent integer value. The allowable digits are 0
through 9, a through f, and A through F.*/

#include<stdio.h>
#include<string.h>
#include<math.h>
#include<limits.h>

double pow(double x, double y);

enum HEXADECIMAL {
    A = 10,
    B,
    C,
    D,
    E,
    F
};

int htoi(char* str){
    if(strlen(str)>2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')){               // checks for trailing '0X' or '0x'
        int res = 0;
        for(int i=strlen(str)-1;i>=2;i--){                                          // scans from end
            if(str[i]>='0' && str[i]<='9'){                                         // checks if it is digit
                res+= (int)pow(16,strlen(str)-i-1) * (str[i]-'0');                  // update result value
            }else{
                if(str[i]=='A' || str[i]=='a'){                                     // checks if letter and updates result correspondingly
                    res+= (int)pow(16,strlen(str)-i-1) * A;
                }else if(str[i]=='B' || str[i]=='b'){
                    res+= (int)pow(16,strlen(str)-i-1) * B;
                }else if(str[i]=='C' || str[i]=='c'){
                    res+= (int)pow(16,strlen(str)-i-1) * C;
                }else if(str[i]=='D' || str[i]=='d'){
                    res+= (int)pow(16,strlen(str)-i-1) * D;
                }else if(str[i]=='E' || str[i]=='e'){
                    res+= (int)pow(16,strlen(str)-i-1) * E;
                }else if(str[i]=='F' || str[i]=='f'){
                    res+= (int)pow(16,strlen(str)-i-1) * F;
                }
            }
        }
        return res;                                                                  //returns result(integer)
    }else{
        return -1;                                                                   // if does'nt begin with either '0x' or '0X' returns -1(error)
    }
}

int main(){
    printf("%d\n",htoi("0X8FA"));
    return 0;
}