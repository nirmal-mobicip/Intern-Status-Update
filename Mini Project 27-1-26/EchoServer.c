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

} Client_FD;

Client_FD cfds[MAX_SOCKETS]; // client echo count info
int n_sockets = 0;           // no of clients connected

long total_echo = 0; // total count of echoes

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void *thread_routene(void *arg)
{
    int client_socket = *(int *)arg;
    int flag = 1;

    while (flag)
    {
        char cbuffer[CLIENT_BUFFER_SIZE], sbuffer[SERVER_BUFFER_SIZE];

        if (cfds[client_socket].fd == -1) // add new client to array
        {
            cfds[client_socket].fd = client_socket;
        }

        int n;
        if ((n = recv(client_socket, cbuffer, CLIENT_BUFFER_SIZE, 0)) > 0)
        {
            Client_Data cdata;
            Server_Data sdata;
            unpack_client_data(&cdata, cbuffer);

            if (strncmp("exit", cdata.message, 4) == 0)
            {
                printf("Socket connection %d Closed\n", client_socket);
                flag = 0;
                cfds[client_socket].fd = -1;
                cfds[client_socket].echo_count = 0;
                close(client_socket);
                break;
            }

            pthread_mutex_lock(&lock);
            cfds[client_socket].echo_count += 1; // increment echo count for particular client
            total_echo++;                        // increment total echoes for server shared variable so mutex lock and unlock
            pthread_mutex_unlock(&lock);

            sdata.echo = cdata;
            sdata.user_count = cfds[client_socket].echo_count;

            pthread_mutex_lock(&lock);
            sdata.total_count = total_echo;
            pthread_mutex_unlock(&lock);

            pack_server_data(&sdata, sbuffer);

            if (send(client_socket, sbuffer, SERVER_BUFFER_SIZE, 0) == -1)
            {
                perror("send()");
            }
            else
            {
                printf("Echo Sent to client : %d\n", client_socket);
            }
        }
        else if (n == 0)
        {
            printf("Socket connection %d Closed\n", client_socket);
            flag = 0;
            cfds[client_socket].fd = -1;
            cfds[client_socket].echo_count = 0;
            close(client_socket);
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
    }

    int server_socket;
    struct addrinfo hints, *res, *p;

    hints.ai_family = AF_UNSPEC;
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
        int *cfd = (int*)malloc(sizeof(int));
        *cfd = client_socket;
        pthread_create(&pid, NULL, thread_routene, cfd); // create new thread for each client
    }

    close(server_socket);
    return 0;
}
