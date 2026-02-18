#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>    // for sys calls like write
// socket
#include <sys/socket.h>
#include <netdb.h>

// epoll
#include <sys/epoll.h>

// file control
#include <fcntl.h>

// error
#include <errno.h>

#define MAX_EVENTS 10
#define PORT "8080"
#define MAX_BUF_SIZE 8192

void close_connection(int epfd, int fd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    printf("Client Connection Closed : %d\n", fd);
    return;
}

void close_server(char *reason)
{
    perror(reason);
    exit(EXIT_FAILURE);
}

void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int add_to_epoll(int epfd,int fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        return 0;
    }
    return 1;
}

int get_listen_socket()
{
    int fd;

    struct addrinfo hints = {0}, *res, *p;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        close_server("getaddrinfo()");
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        setNonBlock(fd);

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(fd, p->ai_addr, p->ai_addrlen) < 0){
            close(fd);
            continue;
        }

        if (listen(fd, 10) < 0){
            close(fd);
            continue;
        }
        break;
    }

    freeaddrinfo(res);
    
    if(p==NULL){
        return -1;
    }
    
    return fd;
}


int main(int argc, char *argv[])
{

    // create epoll fd and event
    int epfd;
    struct epoll_event event[MAX_EVENTS];
    if ((epfd = epoll_create1(0)) == -1)
    {
        close_server("epoll_create1()");
    }

    // create listen socket
    int listen_fd;
    
    if((listen_fd = get_listen_socket())<0){
        close_server("get_listen_socket()");
    }

    // add to epoll
    if(!add_to_epoll(epfd,listen_fd)){
        close_server("add_to_epoll()");
    }

    printf("Server Started and listening at port %s...\n",PORT);
    while(1){

        int nfds = epoll_wait(epfd, event, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close_server("epoll_wait()");
        }

        for(int i=0;i<nfds;i++){
            int fd = event[i].data.fd;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                close_connection(epfd, fd);
                continue;
            }

            if(fd == listen_fd){        // if listen socket

                int client_fd;
                if((client_fd = accept(listen_fd,NULL,NULL))<0){
                    continue;
                }
                
                setNonBlock(client_fd);

                add_to_epoll(epfd,client_fd);

                printf("New Client Connected : %d\n",client_fd);


            }else{                      // if client socket

                char buffer[MAX_BUF_SIZE];
                int n;
                if((n = recv(fd,buffer,MAX_BUF_SIZE,0))>0){
                    write(STDOUT_FILENO,buffer,n);
                    putchar('\n');
                }   

            }

        }

    }



    return 0;
}