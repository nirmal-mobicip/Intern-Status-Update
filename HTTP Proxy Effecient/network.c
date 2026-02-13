#include <stddef.h>
#include <fcntl.h>

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

char *recv_http_request(int fd, int *out_len)
{
    int capacity = 8192;
    int total = 0;
    char *buffer = malloc(capacity);

    if (!buffer) return NULL;

    int header_end = -1;

    while (1)
    {
        if (total >= capacity)
        {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer) return NULL;
        }

        int n = recv(fd, buffer + total, capacity - total, 0);
        if (n <= 0) return NULL;

        total += n;

        // search for end of headers
        for (int i = 0; i < total - 3; i++)
        {
            if (buffer[i] == '\r' && buffer[i+1] == '\n' &&
                buffer[i+2] == '\r' && buffer[i+3] == '\n')
            {
                header_end = i + 4;
                break;
            }
        }

        if (header_end != -1)
            break;
    }

    // Now parse Content-Length if present
    int content_length = 0;
    char *cl = strstr(buffer, "Content-Length:");
    if (cl)
        content_length = atoi(cl + 15);

    // Read body if needed
    while (total < header_end + content_length)
    {
        int n = recv(fd, buffer + total, capacity - total, 0);
        if (n <= 0) return NULL;
        total += n;
    }

    *out_len = total;
    return buffer;
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