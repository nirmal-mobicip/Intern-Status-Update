#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MYPORT "8080"
#define BACKLOG 10

void getClientIP(struct sockaddr_storage *client_addr, char *ipaddress)
{
    void *addr;
    if (client_addr->ss_family == AF_INET)
    {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)client_addr;
        addr = &(ipv4->sin_addr);
    }
    else if (client_addr->ss_family == AF_INET6)
    {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)client_addr;
        addr = &(ipv6->sin6_addr);
    }
    else
    {
        strcpy(ipaddress, "Unknown Address Family");
        return;
    }
    inet_ntop(client_addr->ss_family, addr, ipaddress, INET6_ADDRSTRLEN);
}

int main(int argc, char *argv[])
{

    struct addrinfo hints, *res;

    int enable = 1;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, MYPORT, &hints, &res) != 0)
    {
        printf("Failure at getaddrinfo()\n");
        exit(1);
    }


    printf("Server family: %s\n",
           res->ai_family == AF_INET ? "IPv4" : res->ai_family == AF_INET6 ? "IPv6"
                                                                           : "Other");

    int sockfd;
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    {
        printf("Failure at socket()\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
    {
        printf("Failure at setsockopt()\n");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        printf("Failure at bind()\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        printf("Failure at listen()\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int clientfd;
    if ((clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) == -1)
    {
        printf("Failure at listen()\n");
        exit(EXIT_FAILURE);
    }

    char ipaddress[INET6_ADDRSTRLEN];
    getClientIP(&client_addr, ipaddress);
    printf("Client connected IP : %s\n", ipaddress);
    printf("Response Sent!\n");
    char *str = "Client is connected!";
    if (send(clientfd, str, strlen(str), 0) == -1)
    {
        printf("Failure at send()\n");
        exit(EXIT_FAILURE);
    }

    close(clientfd);
    close(sockfd);

    return 0;
}
