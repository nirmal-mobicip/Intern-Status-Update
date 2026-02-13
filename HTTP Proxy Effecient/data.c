typedef struct conn
{
    int scheme;
    int fd1;
    int fd2;
    int refs;
    int authorized;
} Connection;

typedef struct data
{
    int fd;
    int tunnel_mode;
    Connection *connection;
} Data;

Data *createData(int fd, Connection *c)
{
    Data *d = (Data *)malloc(sizeof(Data));
    d->fd = fd;
    d->connection = c;
    d->tunnel_mode = 0;
    return d;
}

Connection *createConnection(int client, int server)
{
    Connection *c = (Connection *)malloc(sizeof(Connection));
    c->fd1 = client;
    c->fd2 = server;
    c->refs = 0;
    c->authorized = 0;
    return c;
}

void cleanup_connection(int epfd, Data *d)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, d->fd, NULL);
    close(d->fd);
    d->connection->refs--;
    printf("Peer Connection Closed : %d\n", d->fd);
    if (d->connection->refs == 0)
    {
        printf("Pair Full Connection Closed : %d,%d\n", d->connection->fd1, d->connection->fd2);
        free(d->connection);
    }
    free(d);
    return;
}