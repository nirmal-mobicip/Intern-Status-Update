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
#include <pthread.h>
#include <time.h>

#include "datapack.h"

#define PORT "8080"
#define BACKLOG 10

#define MAX_SOCKETS 100

typedef struct client_fd
{

    int fd;
    int echo_count;
    struct sockaddr_storage addr;
    char ipaddress[INET6_ADDRSTRLEN];

} Client_FD;

Client_FD cfds[MAX_SOCKETS]; // client echo count info
int n_sockets = 0;           // no of clients connected

long total_echo = 0; // total count of echoes

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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

int addOrUpdateUser(Client_FD *cfd)
{
    pthread_mutex_lock(&lock);
    int found = 0, i, res;
    for (i = 0; i < n_sockets; i++)
    {
        if (strcmp(cfd->ipaddress, cfds[i].ipaddress) == 0)
        {
            found = 1;
            break;
        }
    }
    if (found)
    {
        if (cfds[i].fd != -1)
        {
            shutdown(cfds[i].fd, SHUT_RDWR); // shutdown old thread socket
            close(cfds[i].fd);               // close old thread socket
            printf("Active Client with old session closed!!\n");
        }
        cfds[i].fd = cfd->fd;
        cfds[i].addr = cfd->addr;
        res = i;
        printf("Exisiting Client Connected : %s\n", cfds[res].ipaddress);
    }
    else
    {
        cfds[n_sockets].addr = cfd->addr;
        cfds[n_sockets].fd = cfd->fd;
        cfds[n_sockets].echo_count = cfd->echo_count;
        strcpy(cfds[n_sockets].ipaddress, cfd->ipaddress);
        res = n_sockets;
        n_sockets++;
        printf("New Client Connected : %s\n", cfds[res].ipaddress);
    }
    pthread_mutex_unlock(&lock);
    return res;
}

void *thread_routene(void *arg)
{
    Client_FD *cfd = (Client_FD *)arg;
    int flag = 1;

    int pos = addOrUpdateUser(cfd);
    
    

    while (flag)
    {
        // if (cfds[pos].fd != cfd->fd)
        // {
        //     printf("Same user logged in with other terminal\nTerminating this session....\n");
        //     close(cfd->fd);
        //     printf("Connection Closed!\n");
        //     break;
        // }
        char cbuffer[CLIENT_BUFFER_SIZE], sbuffer[SERVER_BUFFER_SIZE];
        int n;
        if ((n = recvall(cfd->fd, cbuffer, CLIENT_BUFFER_SIZE, 0)) > 0)
        {
            Client_Data cdata;
            Server_Data sdata;
            unpack_client_data(&cdata, cbuffer);

            if (strncmp("exit", cdata.message, 4) == 0)
            {
                printf("Socket connection %d Closed\n", cfd->fd);
                flag = 0;
                close(cfd->fd);
                cfds[pos].fd = -1;
                break;
            }

            pthread_mutex_lock(&lock);
            cfds[pos].echo_count += 1; // increment echo count for particular client
            total_echo++;              // increment total echoes for server shared variable so mutex lock and unlock
            pthread_mutex_unlock(&lock);

            sdata.echo = cdata;

            pthread_mutex_lock(&lock);
            sdata.user_count = cfds[pos].echo_count;
            sdata.total_count = total_echo;
            pthread_mutex_unlock(&lock);

            pack_server_data(&sdata, sbuffer);

            if (sendall(cfd->fd, sbuffer, SERVER_BUFFER_SIZE, 0) == -1)
            {
                perror("send()");
            }
            else
            {
                char *message = trim(cdata.message);
                printf("Echo Client : %s\n", cfds[pos].ipaddress);
                printf("Echo Message : %s\n", message);
                printf("Echo Time : %s\n", cdata.timeStr);
                putchar('\n');
                free(message);
            }
        }
        else if (n == 0)
        {
            printf("Socket Connection Closed : %s\n", cfd->ipaddress);
            flag = 0;
            close(cfd->fd);
            cfds[pos].fd = -1;
            break;
        }
        else
        {
            perror("recv()");
            exit(EXIT_FAILURE);
        }
    }
    free(arg);
    return NULL;
}

int main()
{

    int enable = 1;

    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        cfds[i].echo_count = 0;
        cfds[i].fd = -1;
        memset(&cfds[i].addr, 0, sizeof(cfds[i].addr));
    }

    int server_socket;
    struct addrinfo hints, *res, *p;

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        perror("getaddrinfo()");
    }

    for (p = res; p != NULL; p = p->ai_next)
    {

        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }

        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
        {
            perror("setsockopt()");
            exit(EXIT_FAILURE);
        }

        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        printf("Some Error Occured!\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    if (listen(server_socket, BACKLOG) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    printf("Echo Server Listening on port %s\n", PORT);

    while (1)
    {
        struct sockaddr_storage remote_info;
        socklen_t len = sizeof(remote_info);
        int client_socket;
        if ((client_socket = accept(server_socket, (struct sockaddr *)&remote_info, &len)) == -1)
        {
            perror("accept()");
            continue;
        }

        pthread_t pid;

        Client_FD *cfd = (Client_FD *)malloc(sizeof(Client_FD));
        cfd->fd = client_socket;
        cfd->addr = remote_info;
        cfd->echo_count = 0;
        getClientIP(&remote_info, cfd->ipaddress);
        pthread_create(&pid, NULL, thread_routene, cfd); // create new thread for each client
    }

    close(server_socket);
    return 0;
}
