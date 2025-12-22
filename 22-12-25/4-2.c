#include<stdio.h>

double my_atof(char str[]){
    double res = 0;
    int i=0, sign,divide=1,power=0;
    while(str[i]!='\0' && str[i]==' '){
        i++;
    }
    if(str[i]=='-'){
        sign = -1;
        i++;
    }else{
        sign = 1;
    }
    while(str[i]!='\0' && str[i]!='e' && str[i]!='E'){
        if(str[i]=='.'){
            if(power!=0) return -1;
            power = 1;
            i++;
            continue;
        }
        if(power){
            divide*=10;
        }
        if(str[i]>='0' && str[i]<='9'){
            res = ((res*10) + (str[i]-'0'));
        }else{
            return -1;
        }
        i++;
    }
    if(str[i]=='E' || str[i]=='e'){
        i++;
        int negative=0,exp=0,value=1;
        if(str[i]=='-'){
            negative = 1;
            i++;
        }else if(str[i]=='+'){
            i++;
        }
        while(str[i]!='\0'){
            if(str[i]>='0' && str[i]<='9'){
                exp = (exp*10) + (str[i]-'0');
            }else{
                return -1;
            }
            i++;
        }
        for(int i=0;i<exp;i++){
            value*=10;
        }
        res = (negative) ? res/value : res*value;
    }
    return sign*res/divide;
}

int main(){

    printf("%lf\n",my_atof("-143.23e-3"));

    return 0;
}