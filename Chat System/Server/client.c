#include <stddef.h>

typedef struct client{
    int fd;
    char* username;
    int active : 1;
    int added : 1;
}Client;

Client* createClient(int fd){
    Client* c = (Client*)malloc(sizeof(Client));
    c->fd = fd;
    c->username = NULL;
    c->active = 1;
    c->added = 0;
    return c;
}