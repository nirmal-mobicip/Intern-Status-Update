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

#define IP "localhost"
#define PORT "8080"

int get_server_socket(struct addrinfo *res)
{
    struct addrinfo *p;
    int sockfd;
    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
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

int main()
{

    fd_set readfds, master;
    FD_ZERO(&readfds);
    FD_ZERO(&master);

    int nfds = 0;

    FD_SET(STDIN_FILENO, &master);
    nfds = STDIN_FILENO + 1;

    struct addrinfo hints, *res, *p;

    int clientfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(IP, PORT, &hints, &res);

    clientfd = get_server_socket(res);

    freeaddrinfo(res);

    if (connect(clientfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        printf("Failed to connect\n");
        exit(EXIT_FAILURE);
    }

    FD_SET(clientfd, &master);

    nfds = clientfd + 1;
    char buff[100];
    while (1)
    {
        readfds = master;
        int res = select(nfds, &readfds, NULL, NULL, NULL);

        if (res > 0)
        {

            for (int i = 0; i < nfds; i++)
            {

                if (FD_ISSET(i, &readfds) && i == STDIN_FILENO)
                {
                    int n = read(i, buff, sizeof(buff));
                    buff[n] = '\0';
                    if (send(clientfd, buff, n, 0) == -1)
                    {
                        printf("Error Sending to server\n");
                    }
                }
                else if (FD_ISSET(i, &readfds) && i == clientfd)
                {
                    int n = recv(i, buff, sizeof(buff), 0);
                    if (n == 0)
                    {
                        printf("Connection Closed\n");
                        close(i);
                        FD_CLR(i, &readfds);
                    }
                    else if (n < 0)
                    {
                        printf("Error Occured while recv()\n");
                        close(i);
                        FD_CLR(i, &readfds);
                    }
                    else
                    {
                        buff[n] = '\0';
                        printf("Broadcast Message : %s\n", buff);
                    }
                }
            }
        }
    }

    return 0;
}