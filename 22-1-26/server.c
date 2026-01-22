#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "8080"
#define BACKLOG 10
#define MAXCLIENTS 100

void getClientIP(struct sockaddr_storage *addr, char *ip)
{
    void *src;
    if (addr->ss_family == AF_INET)
        src = &((struct sockaddr_in *)addr)->sin_addr;
    else
        src = &((struct sockaddr_in6 *)addr)->sin6_addr;

    inet_ntop(addr->ss_family, src, ip, INET6_ADDRSTRLEN);
}

int main(void)
{
    struct addrinfo hints, *res;
    struct pollfd fds[MAXCLIENTS];
    int nfds = 1;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IPv4 + IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        exit(1);
    }

    int listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if (bind(listenfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    fds[0].fd = listenfd;
    fds[0].events = POLLIN;

    printf("Server listening on port %s\n", PORT);

    while (1)
    {
        if (poll(fds, nfds, -1) == -1)
        {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < nfds; i++)
        {
            if (!(fds[i].revents & POLLIN))
                continue;

            if (fds[i].fd == listenfd)
            {
                struct sockaddr_storage client;
                socklen_t len = sizeof client;
                int clientfd = accept(listenfd, (struct sockaddr *)&client, &len);

                if (clientfd == -1)
                {
                    perror("accept");
                    continue;
                }

                if (nfds < MAXCLIENTS)
                {
                    fds[nfds].fd = clientfd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    char ip[INET6_ADDRSTRLEN];
                    getClientIP(&client, ip);
                    printf("Client connected: %s\n", ip);
                }
                else
                {
                    close(clientfd);
                }
            }
            else
            {
                char buf[256];
                int n = recv(fds[i].fd, buf, sizeof buf, 0);

                if (n <= 0)
                {
                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                }
                else
                {
                    const char *msg = "Client is connected!";
                    send(fds[i].fd, msg, strlen(msg), 0);

                    close(fds[i].fd);
                    fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                }
            }
        }
    }
}
