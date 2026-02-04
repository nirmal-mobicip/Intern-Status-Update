#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/epoll.h>

#define MAX_EVENTS 10

int main(){
    struct epoll_event ev,events[MAX_EVENTS];

    int epoll_fd = epoll_create1(0);
    
    if(epoll_fd==-1){
        perror("epoll_create1()");
        exit(0);        
    }

    memset(&ev,0,sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = 0;
    
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,0,&ev)==-1){
        perror("epoll_ctl()");
        exit(0);
    }

    while(1){
        int n = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if(n==-1){
            perror("epoll_wait()");
            exit(0);
        }
        for(int i=0;i<n;i++){
            char buff[100];
            int len = read(events[i].data.fd,buff,sizeof(buff));
            write(1,buff,len);
        }
    }




    return 0;
}