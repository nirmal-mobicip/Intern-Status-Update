// Exercise 5-10. Write the program expr, which evaluates a reverse Polish expression from the
// command line, where each operator or operand is a separate argument. For example,
// expr 2 3 4 + *
// evaluates 2 * (3+4).

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

int stack[100];
int ptr=0;

int main(int argc, char* argv[]){
    argv++;
    argc--;
    int f=1,i=0;
    while(i++<argc){
        if(isdigit(*argv[0]) || (*argv[0]=='-' && isdigit((unsigned char)(*argv)[1]))){
            stack[ptr++] = atoi(*argv);
        }
        else{
            if(ptr>=2){
                int arg1 = stack[--ptr];
                int arg2 = stack[--ptr];
                if(*argv[0]=='+'){
                    stack[ptr++] = arg2+arg1;
                }
                else if(*argv[0]=='-'){
                    stack[ptr++] = arg2-arg1;
                }
                else if(*argv[0]=='*'){
                    stack[ptr++] = arg2*arg1;
                }
                else if(*argv[0]=='/'){
                    if(arg1!=0){
                        stack[ptr++] = arg2/arg1;
                    }
                    else{
                        printf("DIVIDE BY ZERO ERROR!!\n");
                        f=0;
                        break;
                    }
                }
            }
            else{
                printf("INVALID!!\n");
                f=0;
                break;
            }
        }
        argv++;
    }
    if(f && ptr==1){
        printf("RESULT = %d\n",stack[--ptr]);
    }else{
        printf("ERROR OCCURED\n");
    }
    return 0;
}