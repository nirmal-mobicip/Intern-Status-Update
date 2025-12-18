/*Exercise 1.15. Rewrite the temperature conversion program of Section 1.2 to use a function
for conversion.*/

#include<stdio.h>

float fahrenheit_to_celsius(float fah){
    float cel = (5*(fah-32))/9;
    return cel;
}

int main(){
    printf("%.2f\n",fahrenheit_to_celsius(100));
    return 0;
}