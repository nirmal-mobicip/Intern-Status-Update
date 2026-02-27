#include <stddef.h>


typedef struct match{
    int matchID;
    int playerX;
    int playerO;
    char board[3][3];
}Match;

typedef struct client{
    int fd;
    char* username;
    int active : 1;
    int connected : 1;
    Match* currentMatch;
}Client;

Match* createMatch(int playerX,int playerO,int mid){
    Match* m = (Match*)malloc(sizeof(Match));
    m->matchID = mid;
    m->playerX = playerX;
    m->playerO = playerO;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m->board[i][j] = ' ';
        }
    }
    return m;
}

Client* createClient(int fd,Match* m){
    Client* c = (Client*)malloc(sizeof(Client));
    c->fd = fd;
    c->username = NULL;
    c->active = 1;
    c->connected = 0;
    c->currentMatch = m;
    return c;
}