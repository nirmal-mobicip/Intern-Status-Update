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

#define LISTEN_PORT "8080"
#define BUF_SIZE 16384

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

static int send_all(int fd, const char *buf, int len)
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

static void forward_http_request(int server_fd, Request *req, const char *buf, int total)
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

/* ===================== TUNNEL ====================== */

void *tunneler(void *arg)
{
    Pipe *fd = (Pipe *)arg;
    char buffer[BUF_SIZE];
    int n;

    while (1)
    {
        n = recv(fd->from, buffer, sizeof(buffer), 0);
        if (n == 0)
            break; // peer closed cleanly
        if (n < 0)
            break;
        if (send_all(fd->to, buffer, n) < 0)
            break;
    }

    shutdown(fd->to, SHUT_WR);
    shutdown(fd->from, SHUT_RD);
    printf("Connection Closed\n");
    return NULL;
}

void tunnel_https(int client_fd, int server_fd)
{
    pthread_t t1, t2;
    Pipe *c2s = malloc(sizeof(*c2s));
    Pipe *s2c = malloc(sizeof(*s2c));

    *c2s = (Pipe){client_fd, server_fd};
    *s2c = (Pipe){server_fd, client_fd};

    pthread_create(&t1, NULL, tunneler, c2s);
    pthread_create(&t2, NULL, tunneler, s2c);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    close(client_fd);
    close(server_fd);
}

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static char *normalize_url(const char *scheme, const char *url)
{
    if (strstr(url, "://"))
        return strdup(url);

    char *full = malloc(strlen(scheme) + strlen(url) + 4);
    sprintf(full, "%s://%s", scheme, url);
    return full;
}

static int parse_url(const char *scheme, const char *url, URL *out)
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

static int parse_request(char *buf, Request *req)
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

void *process(void *arg)
{
    int client_fd = *(int *)arg;
    char buf[BUF_SIZE];
    int total = 0, n;
    while ((n = recv(client_fd, buf + total, BUF_SIZE - total, 0)) > 0)
    {
        total += n;
        if (strstr(buf, "\r\n\r\n"))
            break;
    }

    write(STDOUT_FILENO, buf, total);

    Request req;
    char *req_copy = strndup(buf, total);
    if (parse_request(req_copy, &req) < 0)
    {
        close(client_fd);
        free(req_copy);
        return NULL;
    }
    int server_fd = connect_upstream(req.url.host, req.url.port);
    if (server_fd < 0)
    {
        close(client_fd);
        free(req_copy);
        return NULL;
    }

    if (!strcmp(req.method, "CONNECT"))
    {
        if (send(client_fd, "HTTP/1.1 200 Connection Established\r\n\r\n", 39, 0) <= 0)
        {
            printf("Error making connection\n");
        }
        tunnel_https(client_fd, server_fd);
        free(req_copy);
        free(arg);
        return NULL;
    }

    forward_http_request(server_fd, &req, buf, total); // sends the packet or first chunk

    char *body = strstr(buf, "\r\n\r\n"); // remaining sent if available
    if (body)
    {
        body += 4;
        int body_len = total - (body - buf);
        if (body_len > 0)
            send_all(server_fd, body, body_len);
    }

    while ((n = recv(server_fd, buf, BUF_SIZE, 0)) > 0)
        send_all(client_fd, buf, n);

    free(req_copy);
    free(arg);
    return NULL;
}

int main(void)
{

    // signal(SIGPIPE, SIG_IGN);

    int listen_fd, client_fd, server_fd;
    char buf[BUF_SIZE];
    int total = 0, n;
    char *hdr_end;

    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, LISTEN_PORT, &hints, &res) != 0)
        die("getaddrinfo");

    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_fd < 0)
        die("socket");

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0)
        die("bind");

    if (listen(listen_fd, 10) < 0)
        die("listen");

    freeaddrinfo(res);

    while (1)
    {
        total = 0;
        client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0)
            continue;

        pthread_t p;
        int *cfd = (int *)malloc(sizeof(int));
        *cfd = client_fd;
        pthread_create(&p, NULL, process, cfd);
    }

    close(listen_fd);

    return 0;
}
