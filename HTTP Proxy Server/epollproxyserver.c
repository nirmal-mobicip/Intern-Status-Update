#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <openssl/evp.h>
#include <errno.h>

#include "hashmap.c"
#include "request.c"
#include "proxy-auth.c"

#define LISTEN_PORT "8080"
#define BUF_SIZE 16384
#define MAX_EVENTS 10

int n = 0;

typedef struct conn
{
    int scheme;
    int fd1;
    int fd2;
    int refs;
} Connection;

typedef struct data
{
    int fd;
    Connection *connection;
} Data;

int send_all(int fd, const char *buf, int len)
{
    int sent = 0;
    while (sent < len)
    {
        int n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0)
            return -1;
        sent += n;
    }
    return sent;
}

int recv_all(int fd, char *buf, int maxlen)
{
    int total = 0;
    int n;

    while ((n = recv(fd, buf + total, maxlen - total, 0)) > 0)
    {
        total += n;

        if (total >= maxlen)
            break;
    }

    if (n < 0)
        return -1;

    return total;
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

        if(buffer)

        total += n;
    }

    *out_len = total;
    return buffer;
}

void forward_http_request(int server_fd, Request *req,
                          const char *buf, int total)
{
    char out[BUF_SIZE];
    int n = 0;

    /* Request line */
    n += snprintf(out + n, sizeof(out) - n,
                  "%s %s %s\r\n",
                  req->method,
                  req->url.path ? req->url.path : "/",
                  req->version);

    const char *p = strstr(buf, "\r\n");
    if (!p)
        return;
    p += 2;

    int has_host = 0;

    while (p < buf + total && strncmp(p, "\r\n", 2) != 0)
    {
        const char *line_end = strstr(p, "\r\n");
        if (!line_end)
            break;

        /* Skip Proxy-Connection */
        if (strncasecmp(p, "Proxy-Connection:", 17) == 0)
            goto next;

        /* Skip Connection (we override it) */
        if (strncasecmp(p, "Connection:", 11) == 0)
            goto next;

        if (strncasecmp(p, "Host:", 5) == 0)
            has_host = 1;

        int len = line_end - p + 2;
        memcpy(out + n, p, len);
        n += len;

    next:
        p = line_end + 2;
    }

    if (!has_host)
        n += snprintf(out + n, sizeof(out) - n,
                      "Host: %s\r\n", req->url.host);

    /* Always force close */
    n += snprintf(out + n, sizeof(out) - n,
                  "Connection: close\r\n");

    memcpy(out + n, "\r\n", 2);
    n += 2;

    send_all(server_fd, out, n);
}

void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static int connect_upstream(const char *host, const char *port)
{
    struct addrinfo hints = {0}, *res;
    int fd;

    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port, &hints, &res) != 0)
        return -1;

    fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0 || connect(fd, res->ai_addr, res->ai_addrlen) < 0)
        return -1;

    freeaddrinfo(res);
    return fd;
}

void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
void setBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

void addToEpollInterest(int epfd, Connection *c, Data *d1, Data *d2)
{
    struct epoll_event ev;

    // d1 = (Data *)malloc(sizeof(Data));
    d1->fd = c->fd1;
    d1->connection = c;
    c->refs += 1;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = d1;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd1, &ev) == -1)
    {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    // d2 = (Data *)malloc(sizeof(Data));
    d2->fd = c->fd2;
    d2->connection = c;
    c->refs += 1;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = d2;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd2, &ev) == -1)
    {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }
}

void cleanup_connection(int epfd, Data *d)
{
    if (d->connection->refs != 0)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, d->fd, NULL);
        // close(d->connection->fd1);
        // close(d->connection->fd2);
        close(d->fd);
        d->connection->refs--;
        printf("Peer Connection Closed : %d\n", d->fd);
        if (d->connection->refs == 0)
        {
            printf("Pair Full Connection Closed : %d,%d\n", d->connection->fd1, d->connection->fd2);
            free(d->connection);
        }
        free(d);
    }
    return;
}

int cacheResponse(int server_fd, Request *req)
{
    if (strstr(req->url.host, "firefox"))
    {
        return 0;
    }
    int n;
    char key[100] = "";
    strcat(key, req->method);
    strcat(key, req->url.host);
    strcat(key, req->url.path);
    printf("key : %s\n", key);
    setBlock(server_fd);

    char *resp = recv_dynamic(server_fd, &n);
    put(key, resp, n);

    setNonBlock(server_fd);
    return 1;
}

int RequestCached(Request *req)
{
    char key[100] = "";
    strcat(key, req->method);
    strcat(key, req->url.host);
    strcat(key, req->url.path);
    return isAvailable(key);
}

char *get_response(Request *req, int *len)
{
    char key[100] = "";
    strcat(key, req->method);
    strcat(key, req->url.host);
    strcat(key, req->url.path);
    HashNode *resp = get(key);
    *len = resp->length;
    return strdup(resp->value);
}

int main(void)
{

    // authorized users

    add_user("nirmal", "mobicipintern");
    add_user("mobicipuser", "mobicip2026");

    struct epoll_event ev, event[MAX_EVENTS];

    int epfd = epoll_create1(0);
    if (epfd == -1)
    {
        perror("epoll_create1()");
        exit(EXIT_FAILURE);
    }

    int listen_fd;
    char buf[BUF_SIZE];
    int total = 0, n;
    char *hdr_end;

    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, LISTEN_PORT, &hints, &res) != 0)
        die("getaddrinfo()");

    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_fd < 0)
        die("socket()");

    setNonBlock(listen_fd);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0)
        die("bind()");

    if (listen(listen_fd, 10) < 0)
        die("listen()");

    freeaddrinfo(res);

    Data *d = (Data *)malloc(sizeof(Data));
    d->fd = listen_fd;
    d->connection = NULL;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = d;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
    {
        die("epoll_ctl()");
    }

    while (1)
    {

        int nfds = epoll_wait(epfd, event, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            die("epoll_wait()");
        }

        for (int i = 0; i < nfds; i++)
        {
            Data *temp = (Data *)event[i].data.ptr;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                cleanup_connection(epfd, temp);
                continue;
            }

            if (temp->fd == listen_fd)
            {

                // listen fd task

                // accept client
                int client_fd = accept(listen_fd, NULL, NULL);
                printf("Connections : %d\n", ++n);
                if (client_fd < 0)
                    continue;

                // get packet
                int total = 0, n;
                while ((n = recv(client_fd, buf + total, BUF_SIZE - total, 0)) > 0)
                {
                    total += n;
                    if (strstr(buf, "\r\n\r\n"))
                        break;
                }
                write(STDOUT_FILENO, buf, total);

                if (!authorize(client_fd, buf))
                {
                    continue;
                }

                // connect to server
                Request req;
                char *req_copy = strndup(buf, total);
                if (parse_request(req_copy, &req) < 0)
                {
                    close(client_fd);
                    free(req_copy);
                    continue;
                }
                int server_fd = connect_upstream(req.url.host, req.url.port);
                if (server_fd < 0)
                {
                    close(client_fd);
                    free(req_copy);
                    continue;
                }

                setNonBlock(client_fd);
                setNonBlock(server_fd);

                Connection *c = (Connection *)malloc(sizeof(Connection));
                c->fd1 = client_fd;
                c->fd2 = server_fd;
                c->scheme = (strcmp(req.url.scheme, "http") == 0) ? 0 : 1;
                c->refs = 0;

                Data *d1 = (Data *)malloc(sizeof(Data));
                Data *d2 = (Data *)malloc(sizeof(Data));

                addToEpollInterest(epfd, c, d1, d2);
                printf("Connection Created : %d,%d\n", client_fd, server_fd);

                if (!strcmp(req.url.scheme, "https"))
                {
                    if (send(client_fd, "HTTP/1.1 200 Connection Established\r\n\r\n", 39, 0) <= 0)
                    {
                        printf("Error making connection\n");
                    }
                }
                else
                {
                    if (!RequestCached(&req) && strcmp(req.method, "GET") == 0)
                    {
                        printf("Request not Cached!!...Caching\n\n");

                        forward_http_request(server_fd, &req, buf, total); // sends the packet or first chunk
                        char *body = strstr(buf, "\r\n\r\n");              // remaining sent if available
                        if (body)
                        {
                            body += 4;
                            int body_len = total - (body - buf);
                            if (body_len > 0)
                                send_all(server_fd, body, body_len);
                        }

                        if (strstr(buf, "Connection: close"))
                        {
                            if (!cacheResponse(server_fd, &req))
                            {
                                continue;
                            }
                        }

                        int len;
                        char *response = get_response(&req, &len);

                        if (send(client_fd, response, len, 0) > 0)
                        {
                            printf("Response is Cached and being sent\n");
                            cleanup_connection(epfd, d1);
                            cleanup_connection(epfd, d2);
                        }
                    }
                    else if (RequestCached(&req))
                    {
                        printf("Request is Cached....\n\n");
                        int len;
                        char *response = get_response(&req, &len);
                        if (send_all(client_fd, response, len) > 0)
                        {
                            printf("Response Sent from Cache!\n");
                            cleanup_connection(epfd, d1);
                            cleanup_connection(epfd, d2);
                        }
                    }
                    else
                    {
                        forward_http_request(server_fd, &req, buf, total); // sends the packet or first chunk
                        char *body = strstr(buf, "\r\n\r\n");              // remaining sent if available
                        if (body)
                        {
                            body += 4;
                            int body_len = total - (body - buf);
                            if (body_len > 0)
                                send_all(server_fd, body, body_len);
                        }
                    }
                }
                free(req_copy);
            }
            else
            {
                int from = temp->fd;
                int to = temp->connection->fd1 == from ? temp->connection->fd2 : temp->connection->fd1;
                int scheme = temp->connection->scheme;

                int n;
                if ((n = recv(from, buf, sizeof(buf), 0)) == 0)
                {
                    cleanup_connection(epfd, temp);
                }
                else if (n < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    perror("recv()");
                    cleanup_connection(epfd, temp);
                }
                else
                {
                    if ((n = send(to, buf, n, 0)) == 0)
                    {
                        cleanup_connection(epfd, temp);
                    }
                    else if (n < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                            continue;
                        perror("send()");
                        cleanup_connection(epfd, temp);
                    }
                }
            }
        }
    }
    close(listen_fd);

    return 0;
}
