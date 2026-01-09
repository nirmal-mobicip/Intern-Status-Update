// Exercise 7-3. Revise minprintf to handle more of the other facilities of printf.

#include<stdio.h>
#include<ctype.h>
#include<stdarg.h>
#include<string.h>



void my_printf(char* fmt, ...){
    int ival;
    void *p_val;
    unsigned int u_ival;
    char* sval,cval;
    double fval;
    va_list ap;
    char* p;
    va_start(ap,fmt);
    for(p = fmt;*p;p++){
        if(*p!='%'){
            putchar(*p);
            continue;
        }
        p++;
        int width = 0;
        while(isdigit(*p)){
            width = (width*10) + (*p - '0');
            p++;
        }
        switch(*p){
            case 'd':
                ival = va_arg(ap,int);
                printf("%*d",width,ival);
                break;
            case 'c':
                cval = va_arg(ap,int);          // for char also int is the type
                printf("%*c",width,cval);
                break;
            case 'f':
                fval = va_arg(ap,double);
                printf("%*f",width,fval);
                break;
            case 's':
                sval = va_arg(ap,char*);
                int len = strlen(sval);
                int spaces = (width>len) ? width-len : 0 ;
                // printf("%d,%d\n",width,spaces);
                for(int i=0;i<spaces;i++){                      // manual string alignment for specified width
                    putchar(' ');
                }
                for(;*sval;sval++){
                    putchar(*sval);
                }
                break;
            case 'x':                           // hexadecimal
                ival = va_arg(ap,int);
                printf("%*x",width,ival);
                break;
            case 'o':                           // octal
                ival = va_arg(ap,int);
                printf("%*o",width,ival);
                break;
            case 'u':
                u_ival = va_arg(ap,unsigned int);
                printf("%*u",width,u_ival);
                break;
            case 'p':
                p_val = va_arg(ap,void *);
                printf("%p",p_val);
                break;
            default:
                putchar('%');
                putchar(*p);
                break;
        }
    }
    va_end(ap);
}

int main(){

    char* str = "nirmal";
    int a = 15;
    int* ptr = &a;
    my_printf("%10s %p\n",str,ptr);
    return 0;
}