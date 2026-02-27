#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "hashmap.c"
#include "websocket.c"
#include "frame.c"
#include "message.c"

#define PORT "8080"
#define MAX_BUF_SIZE 65536
#define MAX_EVENTS 10

void die(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int get_listening_socket()
{
    int listen_fd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET6;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        die("getaddrinfo()");
    }

    for (p = res; p != NULL; p = p->ai_next)
    {

        if ((listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        setNonBlock(listen_fd);

        int opt = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(listen_fd, p->ai_addr, p->ai_addrlen) < 0)
        {
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        die("socket() or bind() error");
    }

    if (listen(listen_fd, 10) < 0)
    {
        die("listen()");
    }

    freeaddrinfo(res);
    printf("Server Started and Listening on port : %s\n", PORT);
    return listen_fd;
}

int add_to_epoll(int epfd, Client *c)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = c;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd, &ev) == -1)
    {
        return 0;
    }
    return 1;
}

void close_connection(int epfd, Client *c)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, NULL);
    close(c->fd);
    printf("Client Connection Closed : %d\n", c->fd);
    c->active = 0;
    c->connected = 0;
    return;
}

int main()
{

    int listen_fd = get_listening_socket();

    int epfd;
    struct epoll_event event[MAX_EVENTS];

    if ((epfd = epoll_create1(0)) == -1)
    {
        die("epoll_create1()");
    }

    Client *listen_client = createClient(listen_fd, NULL);

    add_to_epoll(epfd, listen_client);

    while (1)
    {

        int nfds = epoll_wait(epfd, event, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            die("epoll_wait()");
        }

        for (int i = 0; i < nfds; i++)
        {

            Client *c = (Client *)event->data.ptr;
            int fd = c->fd;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                close_connection(epfd, c);
                continue;
            }

            if (fd == listen_fd)
            {

                int client_fd;
                if ((client_fd = accept(listen_fd, NULL, NULL)) < 0)
                {
                    continue;
                }

                char req[MAX_BUF_SIZE];
                int n;
                if ((n = recv(client_fd, req, MAX_BUF_SIZE, 0)) > 0)
                {
                    write(STDOUT_FILENO, req, n);
                    putchar('\n');
                }

                char *key_start = strstr(req, "Sec-WebSocket-Key: ");
                if (!key_start)
                    continue;

                key_start += strlen("Sec-WebSocket-Key: ");
                char *key_end = strstr(key_start, "\r\n");
                if (!key_end)
                    continue;

                char client_key[128];
                snprintf(client_key, sizeof(client_key), "%.*s",
                         (int)(key_end - key_start), key_start);

                char accept_key[64];
                send_handshake(client_fd, client_key);

                setNonBlock(client_fd);

                Client *c = createClient(client_fd, NULL);

                add_to_epoll(epfd, c);

                printf("New Client Connected : %d\n", client_fd);
            }
            else
            {

                char frameBuffer[MAX_BUF_SIZE];
                int n;

                if ((n = recv(fd, frameBuffer, MAX_BUF_SIZE, 0)) <= 0)
                {
                    close_connection(epfd, c);
                    continue;
                }

                int opcode;
                Frame *recvframe = parse_frame(frameBuffer, n, &opcode);
                printFrame(recvframe);
                if (opcode == 8)
                {
                    printf("Client Closed Connection!\n");
                    close_connection(epfd, c);
                    free(recvframe);
                    continue;
                }
                Message* recvmessage = parse_json_string(recvframe->payload_data,recvframe->payload_len);
                printMessage(recvmessage);

            }
        }
    }

    return 0;
}