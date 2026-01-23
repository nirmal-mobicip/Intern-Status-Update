#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "8080"
#define BACKLOG 10

fd_set master;
int nfds = 0;

char *getClientIP(struct sockaddr_storage *client_info)
{
    void *addr;
    char *res = (char *)malloc(INET6_ADDRSTRLEN * sizeof(char));
    if (client_info->ss_family == AF_INET)
    {
        addr = &(((struct sockaddr_in *)client_info)->sin_addr);
    }
    else if (client_info->ss_family == AF_INET6)
    {
        addr = &(((struct sockaddr_in6 *)client_info)->sin6_addr);
    }
    else
    {
        strcpy(res, "UNKNOWN ADDRESS FAMILY");
        return res;
    }
    inet_ntop(client_info->ss_family, addr, res, INET6_ADDRSTRLEN);
    return res;
}

int get_server_socket(struct addrinfo *res)
{
    int enable = 1;
    struct addrinfo *p;
    int sockfd;
    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
        {
            printf("Failure at setsockopt()\n");
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        printf("Failed\n");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int accept_client(struct sockaddr_storage *client_info, int sockfd)
{
    int clientfd;
    int addr_len = sizeof(*client_info);
    if ((clientfd = accept(sockfd, (struct sockaddr *)client_info, &addr_len)) == -1)
    {
        printf("Failed to connect to client\n");
        exit(EXIT_FAILURE);
    }
    FD_SET(clientfd, &master);
    nfds = clientfd + 1;
    printf("Client Connected : %s - %d\n", getClientIP(client_info), clientfd);
    return clientfd;
}

void broadcast(int sender,int serversock,char* buffer)
{
    for (int j = 0; j < nfds; j++)
    {
        if (j != sender && j != serversock && FD_ISSET(j, &master))
        {
            if ((send(j, buffer, strlen(buffer), 0)) == -1)
            {
                printf("Failed to send to socket %d\n", j);
            }
            else
            {
                printf("Sent to %d\n", j);
            }
        }
    }
}

int main(void)
{

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_ZERO(&master);

    struct addrinfo hints, *res;
    struct sockaddr_storage client_info;

    int sockfd, clientfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &res);

    sockfd = get_server_socket(res);

    freeaddrinfo(res);

    listen(sockfd, BACKLOG);

    FD_SET(sockfd, &master);
    nfds = sockfd + 1;

    while (1)
    {
        printf("Listening...\n");
        readfds = master;
        int res = select(nfds, &readfds, NULL, NULL, NULL);
        if (res > 0)
        {
            for (int i = 0; i < nfds; i++)
            {
                if (FD_ISSET(i, &readfds))
                {
                    if (i == sockfd)
                    {
                        clientfd = accept_client(&client_info, sockfd);
                    }
                    else
                    {
                        char buffer[100];
                        int n = recv(i, buffer, sizeof(buffer), 0);
                        if (n <= 0)
                        {
                            printf("Socket Closed\n");
                            close(i);
                            FD_CLR(i, &master);
                        }
                        else
                        {
                            // broadcast
                            buffer[n] = '\0';
                            printf("Broadcast : %s\n", buffer);
                            broadcast(i,sockfd,buffer);
                        }
                    }
                }
            }
        }
        else
        {
            printf("select returned <=0\n");
            continue;
        }
    }

    return 0;
}