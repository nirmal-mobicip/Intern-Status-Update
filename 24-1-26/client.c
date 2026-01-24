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
    int enable = 1, broadcast = 1;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    if (getaddrinfo(argv[1], SERVERPORT, &hints, &res) != 0)
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

    if (setsockopt(sockfd,SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1)
    {
        printf("Failure at setsockopt()\n");
        exit(EXIT_FAILURE);
    }

    
    int status;
    if ((status = sendto(sockfd, argv[2], strlen(argv[2]), 0,res->ai_addr,res->ai_addrlen)) == -1)
    {
        printf("Failure at send()\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Broadcast Sent!\n");
    }

    close(sockfd);

    return 0;
}
