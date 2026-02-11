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
#include <curl/curl.h>
#include <time.h>


typedef struct
{
    char *url; 
    char *scheme;
    char *host;
    char *port;
    char *path;
} URL;

char *normalize_url(const char *scheme, const char *url)
{
    if (strstr(url, "://"))
        return strdup(url);

    char *full = malloc(strlen(scheme) + strlen(url) + 4);
    sprintf(full, "%s://%s", scheme, url);
    return full;
}


int parse_url(const char *scheme, const char *url, URL *out)
{
    CURLU *u = curl_url();
    char *full = normalize_url(scheme, url);
    out->url = strdup(full);

    if (!u || curl_url_set(u, CURLUPART_URL, full, 0) != CURLUE_OK)
        return -1;

    curl_url_get(u, CURLUPART_SCHEME, &out->scheme, 0);
    curl_url_get(u, CURLUPART_HOST, &out->host, 0);
    curl_url_get(u, CURLUPART_PATH, &out->path, 0);

    if (curl_url_get(u, CURLUPART_PORT, &out->port, 0) != CURLUE_OK)
        out->port = strdup(strcmp(out->scheme, "https") == 0 ? "443" : "80");

    curl_url_cleanup(u);
    free(full);
    return 0;
}

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
        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

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


char* prepare_http_packet(URL url,int proxy){
    char* packet = (char*)malloc(100*sizeof(char));
    snprintf(packet,100,"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",proxy ? url.url : url.path,url.host);
    return packet;
}

int main(int argc,char* argv[])
{

    int proxy = 0;
    URL proxy_url,host_url;

    if(argc==4 && strcmp(argv[1],"-x")==0){
        proxy = 1;
    }

    if(proxy){
        if(strstr(argv[2],"http")!=NULL){
            parse_url("http",argv[2],&proxy_url);
        }else{
            parse_url("https",argv[2],&proxy_url);
        }

        if(strstr(argv[3],"http")!=NULL){
            parse_url("http",argv[3],&host_url);
        }else{
            parse_url("https",argv[3],&host_url);
        }
    }else{
        if(strstr(argv[1],"http")!=NULL){
            parse_url("http",argv[1],&host_url);
        }else{
            parse_url("https",argv[1],&host_url);
        }
    }

    int client_socket, flag = 1;
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(proxy ? proxy_url.host:host_url.host,proxy ? proxy_url.port : host_url.port, &hints, &res);
    client_socket = get_server_socket(res);
    freeaddrinfo(res);

    printf("Connected to Server!\n");

    char* request = prepare_http_packet(host_url,proxy);

    send(client_socket,request,strlen(request),0);

    int len;
    char* response = recv_dynamic(client_socket,&len);
    write(STDOUT_FILENO,response,len);

    return 0;
}
