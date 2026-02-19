#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // for sys calls like write
// socket
#include <sys/socket.h>
#include <netdb.h>

// epoll
#include <sys/epoll.h>

// file control
#include <fcntl.h>

// error
#include <errno.h>

// openssl
#include <openssl/sha.h>
#include <openssl/evp.h>

// own files
#include "frame.c"
#include "message.c"
#include "hashmap.c"

#define MAX_EVENTS 10
#define PORT "8080"
#define MAX_BUF_SIZE 65536

char *create_accept_key(const char *client_key)
{
    char *output = (char *)malloc(512 * sizeof(char));
    // globally unique identifier
    const char *guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char combined[256];
    // combine(key+guid)
    sprintf(combined, "%s%s", client_key, guid);
    unsigned char sha1_result[SHA_DIGEST_LENGTH];
    //  compute sha1 for (key+guid)
    SHA1((unsigned char *)combined, strlen(combined), sha1_result);
    // compute base64 for (sha-1(key+guid))
    EVP_EncodeBlock((unsigned char *)output, sha1_result, SHA_DIGEST_LENGTH);
    // output is base64(sha1(key+guid))
    return output;
}

void send_handshake(int client_fd, const char *client_key)
{
    char *accept_key = create_accept_key(client_key);
    char *response = (char *)malloc(512 * sizeof(char));
    sprintf(response, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n", accept_key);
    send(client_fd, response, strlen(response), 0);
    free(accept_key);
    free(response);
}

void send_pong(int fd, unsigned char *payload, int len)
{
    unsigned char frame[128];
    int pos = 0;

    frame[pos++] = 0x8A; // FIN + Pong

    frame[pos++] = len; // server frames not masked

    memcpy(frame + pos, payload, len);
    pos += len;

    send(fd, frame, pos, 0);
}

char *decode_ws_frame(unsigned char *buf, int *len, int *opcode, int *resp)
{
    if (*len < 2)
    {
        *resp = 0;
        return NULL;
    }

    if (buf[0] & 0x80)
    {
        printf("FIN\n");
    }
    else
    {
        printf("NOT-FIN\n");
    }

    *opcode = buf[0] & 0x0F;

    int masked = buf[1] & 0x80;
    int payload_len = buf[1] & 0x7F;
    int pos = 2;

    if (payload_len == 126)
    {
        payload_len = (buf[2] << 8) | buf[3];
        pos += 2;
    }
    else if (payload_len == 127)
    {
        *resp = 0;
        return NULL;
    }

    unsigned char masking_key[4];
    if (masked)
    {
        memcpy(masking_key, buf + pos, 4);
        pos += 4;
    }

    unsigned char *payload = buf + pos;

    if (masked)
    {
        for (int i = 0; i < payload_len; i++)
            payload[i] ^= masking_key[i % 4];
    }

    *len = payload_len;
    *resp = 1;
    return (char *)payload;
}

void close_connection(int epfd, int fd)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    printf("Client Connection Closed : %d\n", fd);
    return;
}

void close_server(char *reason)
{
    perror(reason);
    exit(EXIT_FAILURE);
}

void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int add_to_epoll(int epfd, Client *c)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = c;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, c->fd, &ev) == -1)
    {
        return 0;
    }
    return 1;
}

int get_listen_socket()
{
    int fd;

    struct addrinfo hints = {0}, *res, *p;
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        close_server("getaddrinfo()");
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        setNonBlock(fd);

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(fd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(fd);
            continue;
        }

        if (listen(fd, 10) < 0)
        {
            close(fd);
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (p == NULL)
    {
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[])
{

    initTable();

    // create epoll fd and event
    int epfd;
    struct epoll_event event[MAX_EVENTS];
    if ((epfd = epoll_create1(0)) == -1)
    {
        close_server("epoll_create1()");
    }

    // create listen socket
    int listen_fd;

    if ((listen_fd = get_listen_socket()) < 0)
    {
        close_server("get_listen_socket()");
    }

    Client *c = createClient(listen_fd);

    // add to epoll
    if (!add_to_epoll(epfd, c))
    {
        close_server("add_to_epoll()");
    }

    printf("Server Started and listening at port %s...\n", PORT);
    while (1)
    {

        int nfds = epoll_wait(epfd, event, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            close_server("epoll_wait()");
        }

        for (int i = 0; i < nfds; i++)
        {
            Client *c = (Client *)event->data.ptr;
            int fd = c->fd;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                close_connection(epfd, fd);
                continue;
            }

            if (fd == listen_fd)
            { // if listen socket

                int client_fd;
                if ((client_fd = accept(listen_fd, NULL, NULL)) < 0)
                {
                    continue;
                }

                // recv request packet
                char req[MAX_BUF_SIZE];
                int n;
                if ((n = recv(client_fd, req, MAX_BUF_SIZE, 0)) > 0)
                {
                    write(STDOUT_FILENO, req, n);
                    putchar('\n');
                }

                char *key_start = strstr(req, "Sec-WebSocket-Key: ");
                if (!key_start)
                    continue;

                key_start += strlen("Sec-WebSocket-Key: ");
                char *key_end = strstr(key_start, "\r\n");
                if (!key_end)
                    continue;

                char client_key[128];
                snprintf(client_key, sizeof(client_key), "%.*s",
                         (int)(key_end - key_start), key_start);

                char accept_key[64];
                send_handshake(client_fd, client_key);

                setNonBlock(client_fd);

                Client *c = createClient(client_fd);

                add_to_epoll(epfd, c);

                printf("New Client Connected : %d\n", client_fd);
            }
            else
            { // if client socket

                unsigned char frame[MAX_BUF_SIZE];
                int n;
                if ((n = recv(fd, frame, MAX_BUF_SIZE, 0)) > 0)
                {
                    Frame *f = parse_frame(frame, n);
                    printFrame(*f);
                    Message *m = parse_json_string(f->payload_data, f->payload_len);
                    printMessage(*m);

                    if (!c->added && strcmp("client", m->from) == 0 && strcmp("server", m->to) == 0)
                    {

                        c->added = 1;
                        c->username = strdup(m->message);
                        put(m->message, c);
                        printf("Client Added to Map!\n");
                    }
                    else
                    {
                        HashNode* to;

                        if ((to = get(m->to)) != NULL)
                        {
                            // prepare packet
                            send(to->client->fd, m->message, m->message_len, 0);
                            printf("Message sent to another peer\n");
                        }
                        else
                        {
                            printf("No such User!!\n");
                        }
                    }

                    // free(m);
                    // free(f);
                }
                else
                {
                    printf("Client Disconnected!\n");
                    close_connection(epfd, fd);
                }
            }
        }
    }

    return 0;
}