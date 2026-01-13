#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#include<sys/file.h>

int main(int argc,char* argv[]){

    if(argc!=3){
        printf("Usage : cp <source> <destination>\n");
    }else{
        int f1,f2;
        if((f1 = open(argv[1],O_RDONLY,0))==-1){
            printf("Can't open file : %s\n",argv[1]);
            exit(1);
        }
        if((f2 = creat(argv[2],777))==-1){
            printf("Can't create file : %s\n",argv[2]);
            exit(1);
        }
        char buf[BUFSIZ];
        int n;
        while((n = read(f1,buf,BUFSIZ))>0){
            if(write(f2,buf,n)!=n){
                printf("Can't copy to file : %s\n",argv[1]);
                exit(1);
            }
        }
        printf("Copy Done from : %s to : %s\n",argv[1],argv[2]);
    }

    return 0;
}