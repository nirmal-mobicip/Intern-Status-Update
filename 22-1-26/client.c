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

#define SERVERDOMAIN "localhost"
#define SERVERPORT "8080"

int main(int argc, char *argv[])
{

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVERDOMAIN, SERVERPORT, &hints, &res) != 0)
    {
        printf("Failure at getaddrinfo()\n");
        exit(1);
    }


    int sockfd;
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    {
        printf("Failure at socket()\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        printf("Failure at connect()\n");
        exit(EXIT_FAILURE);
    }

    char buffer[100] = "Hello Server, this is a message from client!";
    int status;
    if ((status = send(sockfd, buffer, strlen(buffer), 0)) == -1)
    {
        printf("Failure at send()\n");
        exit(EXIT_FAILURE);
    }else
    {
        printf("Sent a test message to server");
    }

    if ((status = recv(sockfd, buffer, sizeof(buffer), 0)) == -1)
    {
        printf("Failure at recv()\n");
        exit(EXIT_FAILURE);
    }else
    {
        buffer[status] = '\0';
        printf("Acknowledgement from Server : %s\n",buffer);
    }



    close(sockfd);

    return 0;
}
