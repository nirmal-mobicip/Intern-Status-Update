/*Exercise 1-8. Write a program to count blanks, tabs, and newlines.*/


#include<stdio.h>

int main(){
    int ch;                            // return value of getchar() is int and not char
    int blank = 0, tab = 0, nl = 0;
    
    while((ch=getchar())!=EOF){

        if(ch==' '){
            blank++;
        }else if(ch=='\t'){
            tab++;
        }else if(ch=='\n'){
            nl++;
        }
    }

    printf("Blanks : %d\n",blank);
    printf("Tabs : %d\n",tab);
    printf("Newlines : %d\n",nl);




    return 0;
}