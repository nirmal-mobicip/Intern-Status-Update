/*Exercise 1-5. Modify the temperature conversion program to print the table in reverse order,
that is, from 300 degrees to 0.*/

#include<stdio.h>

int main(){

    int limit = 300;
    int cel = limit;
    
    
    printf("Celsius \t Fahrenheit\n");
    
    while(cel>=0){
        int fah = ((9*cel)/5)+32;
        printf("%7d \t %10d\n",cel,fah);
        cel-=20;
    }

    return 0;
}