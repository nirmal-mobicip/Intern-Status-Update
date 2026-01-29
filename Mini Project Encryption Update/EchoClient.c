#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

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

#define IP "localhost"
#define PORT "8080"

void getCurrentTime(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm t;
    if (localtime_r(&now, &t) == NULL)
    {
        buf[0] = '\0';
        return;
    }
    snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900,
             t.tm_mon + 1, t.tm_mday, t.tm_hour,
             t.tm_min, t.tm_sec);
}

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

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
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

    SSL_library_init();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    int client_socket, flag = 1;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(IP, PORT, &hints, &res);
    client_socket = get_server_socket(res);
    freeaddrinfo(res);

    SSL *client_ssl_socket = SSL_new(ctx);
    SSL_set_fd(client_ssl_socket, client_socket);
    SSL_connect(client_ssl_socket);


    printf("Connected to Server!\n");

    while (flag)
    {
        char str[MAX_TEXT_SIZE];
        printf("Enter Message to echo : ");
        if (fgets(str, sizeof(str), stdin) != NULL)
        {
            str[strcspn(str, "\n")] = '\0';
        }
        else
        {
            perror("stdin");
            exit(EXIT_FAILURE);
        }

        if (strncmp("exit", str, 4) == 0)
        {
            printf("Connection Closed!\n");

            SSL_shutdown(client_ssl_socket);
            SSL_free(client_ssl_socket);

            close(client_socket);
            flag = 0;
            exit(EXIT_SUCCESS);
        }

        Client_Data cdata;
        char timeStr[32];
        getCurrentTime(timeStr, sizeof(timeStr));
        strcpy(cdata.message, str);
        strcpy(cdata.timeStr, timeStr);

        char cbuffer[CLIENT_BUFFER_SIZE];
        pack_client_data(&cdata, cbuffer);

        if (TLS_sendall(client_ssl_socket, cbuffer, CLIENT_BUFFER_SIZE) == -1)
        {
            perror("send()");
            exit(EXIT_FAILURE);
        }

        int n;
        char sbuffer[SERVER_BUFFER_SIZE];
        Server_Data sdata;
        if ((n = TLS_recvall(client_ssl_socket, sbuffer, SERVER_BUFFER_SIZE)) > 0)
        {
            Server_Data received;
            unpack_server_data(&received, sbuffer);
            char *message = trim(received.echo.message);
            printf("Message : %s\n", message);
            printf("Timestamp : %s\n", received.echo.timeStr);
            printf("User Count : %ld\n", received.user_count);
            printf("Total Count : %ld\n", received.total_count);
            putchar('\n');
            free(message);
        }
        else if (n == 0)
        {
            printf("Connection Closed Remotely!\n");

            SSL_shutdown(client_ssl_socket);
            SSL_free(client_ssl_socket);
            
            close(client_socket);
            flag = 0;
        }
        else
        {
            perror("recv()");
            exit(EXIT_FAILURE);
        }
    }
    SSL_CTX_free(ctx);
    return 0;
}
