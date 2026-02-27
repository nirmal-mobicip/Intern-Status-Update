#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include "request.c"
#include "proxy-auth.c"
#include "data.c"
#include "network.c"
#include "hashmap.c"

#define BUF_SIZE 16384
#define MAX_EVENTS 10

void die(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int add_to_epoll(int epfd, Data *d)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = d;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, d->fd, &ev) == -1)
    {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    initTable();

    add_user("nirmal", "mobicipintern");
    add_user("mobicipuser", "mobicip2026");

    struct epoll_event event[MAX_EVENTS];

    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        die("epoll_create1()");
    }

    int listen_fd;

    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, argv[1], &hints, &res) != 0)
        die("getaddrinfo()");

    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_fd < 0)
        die("socket()");

    setNonBlock(listen_fd);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0)
        die("bind()");

    if (listen(listen_fd, 10) < 0)
        die("listen()");

    freeaddrinfo(res);

    Data *d = createData(listen_fd, NULL);

    if (!add_to_epoll(epfd, d))
    {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    printf("Server Started and Listening on port %s\n", argv[1]);
    while (1)
    {

        int nfds = epoll_wait(epfd, event, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("epoll_wait()");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++)
        {
            Data *temp = (Data *)event[i].data.ptr;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                cleanup_connection(epfd, temp);
                continue;
            }

            if (temp->fd == listen_fd)
            { // if it is listening socket

                // accept client
                int client_fd = accept(listen_fd, NULL, NULL);
                if (client_fd < 0)
                    continue;

                // set as non blocking
                setNonBlock(client_fd);

                // create connection and data
                Connection *c = createConnection(client_fd, -1);
                Data *d1 = createData(client_fd, c);

                // add to epoll
                add_to_epoll(epfd, d1);

                printf("Client Connection Created : %d\n", client_fd);
            }
            else
            { // if it is a server or client socket

                if (temp->fd == temp->connection->fd1 && temp->tunnel_mode == 0 && temp->connection->authorized == 0) // if client socket new connection
                {
                    char request_packet[BUF_SIZE];

                    int len = recv(temp->fd, request_packet, sizeof(request_packet), 0);

                    if (len <= 0)
                    {
                        cleanup_connection(epfd, temp);
                        continue;
                    }

                    write(STDOUT_FILENO, request_packet, len); // print request

                    if (!authorize(temp->fd, request_packet))
                    {
                        cleanup_connection(epfd, temp);
                        continue;
                    }
                    else
                    {
                        temp->connection->authorized = 1;
                    }

                    // parse request
                    Request req;
                    char *req_copy = strndup(request_packet, len);
                    if (parse_request(req_copy, &req) < 0)
                    {
                        close(temp->fd);
                        free(req_copy);
                        continue;
                    }

                    // if http GET, then create key for hashmap and check in cache
                    if (strcmp(req.method, "GET") == 0 && strcmp(req.url.scheme, "http") == 0)
                    {
                        temp->connection->key = getKeyfromRequest(&req); // for caching
                        printf("Key : %s\n", temp->connection->key);

                        HashNode *node;
                        if (((node = get(temp->connection->key))) != NULL)
                        {
                            if (send(temp->fd, node->value, node->length, 0) > 0)
                            {
                                printf("Successfully Returned Cached Data\n");
                                cleanup_connection(epfd, temp);
                                break;
                            }
                        }
                    }

                    // connect to server
                    int server_fd = connect_upstream(req.url.host, req.url.port);
                    if (server_fd < 0)
                    {
                        close(temp->fd);
                        free(req_copy);
                        continue;
                    }

                    setNonBlock(server_fd);

                    temp->connection->fd2 = server_fd;
                    temp->connection->scheme = (strcmp(req.url.scheme, "http") == 0) ? 0 : 1;
                    Data *d2 = createData(server_fd, temp->connection);

                    add_to_epoll(epfd, d2);

                    printf("Connection Created : %d <-> %d\n", temp->fd, server_fd);

                    if (strcmp(req.url.scheme, "https") == 0)
                    {
                        if (send(temp->fd, "HTTP/1.1 200 Connection Established\r\n\r\n", 39, 0) <= 0)
                        {
                            printf("Error making connection\n");
                            cleanup_connection(epfd, temp);
                            cleanup_connection(epfd, d2);
                            continue;
                        }
                        else
                        {
                            temp->tunnel_mode = 1;
                        }
                    }
                    else
                    {
                        int n;
                        if ((n = send(server_fd, request_packet, len, 0)) < 0)
                        {
                            perror("send()");
                            cleanup_connection(epfd, temp);
                            cleanup_connection(epfd, d2);
                        }
                        // printf("Request Sent to server!\n");
                    }
                }
                else if (temp->connection->authorized)
                { // just relay

                    int from = temp->fd;
                    int to = temp->connection->fd1 == from ? temp->connection->fd2 : temp->connection->fd1;
                    int scheme = temp->connection->scheme;

                    int n;
                    char buf[BUF_SIZE];

                    if ((n = recv(from, buf, sizeof(buf), 0)) == 0)
                    {
                        cleanup_connection(epfd, temp);
                    }
                    else if (n < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            continue;
                        perror("recv()");
                        cleanup_connection(epfd, temp);
                    }
                    else
                    {
                        // printf("Response Received from server!\n");

                        // cache the response
                        if (temp->connection->key != NULL)
                        {
                            printf("Response Cached!\n");
                            put(temp->connection->key, buf, n);
                        }

                        if ((n = send(to, buf, n, 0)) == 0)
                        {
                            cleanup_connection(epfd, temp);
                        }
                        else if (n < 0)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                                continue;
                            perror("send()");
                            cleanup_connection(epfd, temp);
                        }
                    }
                }
            }
        }
    }

    return 0;
}