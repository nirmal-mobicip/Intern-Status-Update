/*Exercise 1-3. Modify the temperature conversion program to print a heading above the table.*/

#include<stdio.h>

int main(){

    int fah = 0;
    int limit = 300;
    
    printf("Fahrenheit \t Celsius\n");
    
    while(fah<=limit){
        int cel = (5*(fah-32))/9;
        printf("%10d \t %7d\n",fah,cel);
        fah +=20;
    }

    return 0;
}