/*Exercise 1-4. Write a program to print the corresponding Celsius to Fahrenheit table.*/

#include<stdio.h>

int main(){

    int cel = 0;
    int limit = 300;
    
    printf("Celsius \t Fahrenheit\n");
    
    while(cel<=limit){
        int fah = ((9*cel)/5)+32;
        printf("%7d \t %10d\n",cel,fah);
        cel+=20;
    }

    return 0;
}