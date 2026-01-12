#include<stdio.h>

void print_file(FILE* ip, FILE* op){
    int c;
    while((c=getc(ip))!=EOF){
        putc(c,op);
    }
}

int main(int argc, char* argv[]){
    
    FILE *fp;

    while(--argc>0){
        if((fp = fopen(*++argv,"r"))==NULL){
            printf("can't open file : %s\n",*(argv-1));
            return 1;
        }else{
            print_file(fp,stdout);
            fclose(fp);
        }
    }


    return 0;
}