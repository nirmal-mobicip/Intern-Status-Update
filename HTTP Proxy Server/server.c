// #define _POSIX_C_SOURCE 200112L
// #define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <curl/curl.h>

#define MYPORT "8080"
#define BACKLOG 10

typedef struct url_info
{
    char *scheme;
    char *host;
    char *port;
    char *path;
} URL;    
typedef struct packet_info
{
    char *method;
    URL url;
    char *version;
    char *user_agent;
    char *accept;

} Packet;


static char *normalize_url(const char *scheme, const char *url)
{
    if (strstr(url, "://"))
        return strdup(url);

    char *full = malloc(strlen(scheme) + strlen(url) + 4);    
    sprintf(full, "%s://%s", scheme, url);
    return full;
}    

int parse_url(const char *scheme, const char *url, URL *out)
{
    CURLU *u = NULL;
    CURLUcode rc;
    char *full_url = NULL;

    memset(out, 0, sizeof(*out));

    full_url = normalize_url(scheme, url);

    u = curl_url();
    if (!u)
        goto fail;

    if (curl_url_set(u, CURLUPART_URL, full_url, 0) != CURLUE_OK)    
        goto fail;

    curl_url_get(u, CURLUPART_SCHEME, &out->scheme, 0);    
    curl_url_get(u, CURLUPART_HOST, &out->host, 0);

    rc = curl_url_get(u, CURLUPART_PATH, &out->path, 0);
    if (rc != CURLUE_OK)
        out->path = strdup("/");

    rc = curl_url_get(u, CURLUPART_PORT, &out->port, 0);    
    if (rc == CURLUE_NO_PORT)
    {
        out->port = strdup(strcmp(out->scheme, "https") == 0 ? "443" : "80");
    }    

    curl_url_cleanup(u);
    free(full_url);
    return 0;

fail:    
    if (u)
        curl_url_cleanup(u);
    free(full_url);    
    return -1;
}    


void parse_packet(Packet *res, char *buffer)
{
    char* ptr = buffer;
    char* full_url;
    res->method = buffer;   //extract buffer
    while(*ptr!=' '){
        ptr++;
    }
    *ptr = '\0';

    
    ptr++;
    full_url = ptr;         // extract full url+path
    while(*ptr!=' '){
        ptr++;
    }
    *ptr = '\0';
    ptr++;
    res->version = ptr;     // extract version
    while(*ptr!='\r'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;  
    while(*ptr!=' '){
        ptr++;
    }
    ptr++;
    res->url.host = ptr;        // extract domain
    while(*ptr!='\r'){
        ptr++;
    }
    
    *ptr = '\0';
    ptr++;
    while(*ptr!=' '){
        ptr++;
    }
    ptr++;
    res->user_agent = ptr;  // extract user agent
    while(*ptr!='\r'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;
    while(*ptr!=' '){
        ptr++;
    }
    ptr++;
    res->accept = ptr;
    while(*ptr!='\r'){
        ptr++;
    }
    *ptr = '\0';
    
    if(strcmp(res->method,"CONNECT")==0){           // set port
        parse_url("https",full_url,&res->url);
    }else{
        parse_url("http",full_url,&res->url);
    }
}

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
    char buffer[4096];
    int status = 0;
    char *end;
    while (1)
    {
        if ((status = recv(clientfd, buffer + status, sizeof(buffer) - status, 0)) == -1)
        {
            printf("Failure at recv()\n");
            exit(EXIT_FAILURE);
        }
        if (status == 0)
        {
            printf("Server Closed the connection for you\n");
            break;
        }
        else
        {
            if ((end = strstr(buffer, "\r\n\r\n")) != NULL)
            {
                break;
            }
        }
    }
    end += 4;
    *end = '\0';

    int i = 0;
    while (buffer[i] != '\0')
    {
        if (buffer[i] == '\n')
        {
            putchar('\\');
            putchar('n');
            putchar('\n');
        }
        else if (buffer[i] == '\r')
        {
            putchar('\\');
            putchar('r');
        }
        else
        {
            putchar(buffer[i]);
        }
        i++;
    }
    Packet data;
    parse_packet(&data,buffer);
    printf("%s\n",data.method);
    printf("%s\n",data.url.scheme);
    printf("%s\n",data.url.host);
    printf("%s\n",data.url.port);
    printf("%s\n",data.url.path);
    printf("%s\n",data.version);
    printf("%s\n",data.user_agent);
    printf("%s\n",data.accept);
    

    close(clientfd);
    close(sockfd);

    return 0;
}
