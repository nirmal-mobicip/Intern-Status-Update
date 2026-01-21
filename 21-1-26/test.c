#include<stdio.h>
#include<unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>



int main(){
    int a;
    if((a = fork())==-1){
        printf("Failed to create child process\n");
    }else if(a==0){
        printf("I AM AT CHILD\n");
    }else{
        printf("I AM AT PARENT\n");
    }
    printf("Hello\n");
    return 0;
}