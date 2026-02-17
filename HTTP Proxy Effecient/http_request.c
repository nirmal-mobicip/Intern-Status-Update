#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <openssl/evp.h>
#include <time.h>

#include "request.c"

char *recv_dynamic(int fd, int *out_len)
{
    int capacity = 8192;
    int total = 0;
    char *buffer = malloc(capacity);

    if (!buffer)
        return NULL;

    while (1)
    {
        if (total >= capacity)
        {
            capacity *= 2;
            char *tmp = realloc(buffer, capacity);
            if (!tmp)
            {
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }

        int n = recv(fd, buffer + total, capacity - total, 0);

        if (n == 0)
            break; // connection closed

        if (n < 0)
        {
            free(buffer);
            return NULL;
        }

        total += n;
    }

    *out_len = total;
    return buffer;
}

void recv_all(int fd)
{
    char buf[BUFSIZ];
    int n;
    while (1)
    {
        n = recv(fd, buf, sizeof(buf), 0);
        if(n<=0){
            break;
        }else{
            write(STDOUT_FILENO, buf, n);
        }
    }
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

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct timeval tv;
        tv.tv_sec = 3; // 3 seconds timeout
        tv.tv_usec = 0;

        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

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

char *prepare_http_packet(URL url, int proxy, int auth, char *auth_data, char *method, int data, char *d)
{
    if (strcmp(url.scheme, "https") == 0)
    {
        method = strdup("CONNECT");
    }
    char *packet = (char *)malloc(4096 * sizeof(char));
    if (auth)
    {
        unsigned char encoded[256];
        EVP_EncodeBlock(encoded, (unsigned char *)auth_data, strlen(auth_data));
        if (data)
        {
            snprintf(packet, 4096, "%s %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %ld\r\nConnection: close\r\nProxy-Authorization: Basic %s\r\n\r\n%s", method, proxy ? url.url : url.path, url.host, strlen(d), encoded, (data && (strcmp(method, "PUT") == 0 || strcmp(method, "POST") == 0)) ? d : "");
        }
        else
        {
            snprintf(packet, 4096, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nProxy-Authorization: Basic %s\r\n\r\n%s", method, proxy ? url.url : url.path, url.host, encoded, (data && (strcmp(method, "PUT") == 0 || strcmp(method, "POST") == 0)) ? d : "");
        }
    }
    else
    {
        if (data)
        {
            snprintf(packet, 4096, "%s %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n%s", method, proxy ? url.url : url.path, url.host, strlen(d), (data && (strcmp(method, "PUT") == 0 || strcmp(method, "POST") == 0)) ? d : "");
        }
        else
        {
            snprintf(packet, 4096, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n%s", method, proxy ? url.url : url.path, url.host, (data && (strcmp(method, "PUT") == 0 || strcmp(method, "POST") == 0)) ? d : "");
        }
    }
    return packet;
}

int main(int argc, char *argv[])
{

    int proxy = 0, auth = 0, data = 0, host = 0, method = 0;
    URL proxy_url, host_url;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-x") == 0)
        {
            proxy = i;
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            auth = i;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            data = i;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            host = i;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            method = i;
        }
    }

    if (proxy)
    {
        if (strstr(argv[proxy + 1], "http") != NULL)
        {
            parse_url("http", argv[proxy + 1], &proxy_url);
        }
        else
        {
            parse_url("https", argv[proxy + 1], &proxy_url);
        }

        if (strstr(argv[host + 1], "http") != NULL)
        {
            parse_url("http", argv[host + 1], &host_url);
        }
        else
        {
            parse_url("https", argv[host + 1], &host_url);
        }
    }
    else
    {
        if (strstr(argv[host + 1], "http") != NULL)
        {
            parse_url("http", argv[host + 1], &host_url);
        }
        else
        {
            parse_url("https", argv[host + 1], &host_url);
        }
    }

    int client_socket, flag = 1;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(proxy ? proxy_url.host : host_url.host, proxy ? proxy_url.port : host_url.port, &hints, &res);
    client_socket = get_server_socket(res);
    freeaddrinfo(res);

    printf("Connected to Server!\n");

    char *request = prepare_http_packet(host_url, proxy, auth, auth ? argv[auth + 1] : NULL, (method) ? argv[method + 1] : "GET", data, (data) ? argv[data + 1] : NULL);

    send(client_socket, request, strlen(request), 0);

    // int len;
    recv_all(client_socket);
    // write(STDOUT_FILENO, response, len);
    close(client_socket);

    return 0;
}
