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
#include <errno.h>

#define LISTEN_PORT "8080"
#define BUF_SIZE 16384
#define MAX_EVENTS 10

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

typedef struct
{
    char *scheme;
    char *host;
    char *port;
    char *path;
} URL;

typedef struct
{
    char *method;
    char *version;
    URL url;
} Request;

typedef struct
{
    int from;
    int to;
} Pipe;

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

void forward_http_request(int server_fd, Request *req, const char *buf, int total)
{
    char out[BUF_SIZE];
    int n = 0;

    /* Request line */
    n += snprintf(out + n, sizeof(out) - n,
                  "%s %s %s\r\n",
                  req->method,
                  req->url.path ? req->url.path : "/",
                  req->version);

    /* Copy headers line by line */
    const char *p = strstr(buf, "\r\n");
    if (!p)
        return;
    p += 2;

    int has_host = 0;
    int has_connection = 0;

    while (p < buf + total && strncmp(p, "\r\n", 2) != 0)
    {
        const char *line_end = strstr(p, "\r\n");
        if (!line_end)
            break;

        /* Skip Proxy-Connection */
        if (strncasecmp(p, "Proxy-Connection:", 17) == 0)
            goto next;

        if (strncasecmp(p, "Host:", 5) == 0)
            has_host = 1;

        if (strncasecmp(p, "Connection:", 11) == 0)
            has_connection = 1;

        int len = line_end - p + 2;
        memcpy(out + n, p, len);
        n += len;

    next:
        p = line_end + 2;
    }

    if (!has_host)
        n += snprintf(out + n, sizeof(out) - n,
                      "Host: %s\r\n", req->url.host);

    if (!has_connection)
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

int parse_request(char *buf, Request *req)
{
    char *p = buf;

    req->method = p;
    p = strchr(p, ' ');
    if (!p)
        return -1;
    *p++ = '\0';

    char *url = p;
    p = strchr(p, ' ');
    if (!p)
        return -1;
    *p++ = '\0';

    req->version = p;
    p = strstr(p, "\r\n");
    if (!p)
        return -1;
    *p = '\0';

    return parse_url(
        strcmp(req->method, "CONNECT") == 0 ? "https" : "http",
        url,
        &req->url);
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

void addToEpollInterest(int epfd, Connection *c)
{
    struct epoll_event ev;

    Data *d1 = (Data *)malloc(sizeof(Data));
    d1->fd = c->fd1;
    d1->connection = c;
    c->refs+=1;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = d1;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd1, &ev) == -1)
    {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    Data *d2 = (Data *)malloc(sizeof(Data));
    d2->fd = c->fd2;
    d2->connection = c;
    c->refs+=1;
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
    if(d->connection->refs!=0){
        epoll_ctl(epfd, EPOLL_CTL_DEL, d->connection->fd1, NULL);
        epoll_ctl(epfd, EPOLL_CTL_DEL, d->connection->fd2, NULL);
        close(d->fd);
        d->connection->refs--;
        printf("Peer Connection Closed : %d\n",d->fd);
        if(d->connection->refs==0){
            printf("Pair Full Closed connection : %d,%d\n",d->connection->fd1,d->connection->fd2); 
            free(d->connection);   
        }
        free(d);
    }
    return;
}

int main(void)
{
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
                cleanup_connection(epfd,temp);
                continue;
            }

            if (temp->fd == listen_fd)
            {

                // listen fd task

                // accept client
                int client_fd = accept(listen_fd, NULL, NULL);
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

                addToEpollInterest(epfd, c);
                printf("Connection Created : %d,%d\n",client_fd,server_fd);

                if (!strcmp(req.url.scheme, "https"))
                {
                    if (send(client_fd, "HTTP/1.1 200 Connection Established\r\n\r\n", 39, 0) <= 0)
                    {
                        printf("Error making connection\n");
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
