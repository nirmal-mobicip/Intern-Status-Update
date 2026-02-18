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

// openssl
#include <openssl/sha.h>
#include <openssl/evp.h>

#define MAX_EVENTS 10
#define PORT "8080"
#define MAX_BUF_SIZE 8192

void create_accept_key(const char *client_key, char *output)
{
    const char *guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    char combined[256];
    sprintf(combined, "%s%s", client_key, guid);

    unsigned char sha1_result[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)combined, strlen(combined), sha1_result);

    EVP_EncodeBlock((unsigned char*)output, sha1_result, SHA_DIGEST_LENGTH);
}

void send_handshake(int client_fd, const char *accept_key)
{
    char response[512];

    sprintf(response,
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_key);

    send(client_fd, response, strlen(response), 0);
}


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
                
                // recv request packet
                char req[MAX_BUF_SIZE];
                int n;
                if((n = recv(client_fd,req,MAX_BUF_SIZE,0))>0){
                    write(STDOUT_FILENO,req,n);
                    putchar('\n');
                }

                char* client_key = strstr(req,"Sec-WebSocket-Key: ");
                client_key+=19;
                client_key = strndup(client_key,24);
                
                char accept_key[64];
                create_accept_key(client_key,accept_key);
                send_handshake(client_fd,accept_key);
                                
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