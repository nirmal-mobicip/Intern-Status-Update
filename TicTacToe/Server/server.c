#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "client.c"
#include "websocket.c"
#include "frame.c"
#include "message.c"

#define PORT "8080"
#define MAX_BUF_SIZE 65536
#define MAX_EVENTS 10
#define MAX_MATCHES 1000

int match_id = 0;
Match *matches[MAX_MATCHES];

void die(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int areEqual(char i, char j, char k)
{
    if (i == j && i == k)
    {
        return 1;
    }
    return 0;
}

char evaluate_board(char mat[3][3])
{
    // Check rows
    for (int i = 0; i < 3; i++)
    {
        if (mat[i][0] != ' ' &&
            mat[i][0] == mat[i][1] &&
            mat[i][1] == mat[i][2])
            return mat[i][0];
    }

    // Check columns
    for (int j = 0; j < 3; j++)
    {
        if (mat[0][j] != ' ' &&
            mat[0][j] == mat[1][j] &&
            mat[1][j] == mat[2][j])
            return mat[0][j];
    }

    // Check diagonal 1
    if (mat[0][0] != ' ' &&
        mat[0][0] == mat[1][1] &&
        mat[1][1] == mat[2][2])
        return mat[0][0];

    // Check diagonal 2
    if (mat[0][2] != ' ' &&
        mat[0][2] == mat[1][1] &&
        mat[1][1] == mat[2][0])
        return mat[0][2];

    return '*'; // no winner found
}
int get_listening_socket()
{
    int listen_fd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET6;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        die("getaddrinfo()");
    }

    for (p = res; p != NULL; p = p->ai_next)
    {

        if ((listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            continue;
        }

        setNonBlock(listen_fd);

        int opt = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(listen_fd, p->ai_addr, p->ai_addrlen) < 0)
        {
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        die("socket() or bind() error");
    }

    if (listen(listen_fd, 10) < 0)
    {
        die("listen()");
    }

    freeaddrinfo(res);
    printf("Server Started and Listening on port : %s\n", PORT);
    return listen_fd;
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
void send_peer_close_response_to_another_peer(Match *m, int fd)
{
    if (m->players == 2)
    {
        int opponent;

        m->players--;

        if (m->playerX == fd)
            opponent = m->playerO;
        else
            opponent = m->playerX;

        if (opponent != -1)
        {
            // Opponent wins by disconnect
            uint8_t response_frame_buffer[1024];
            int payload_len, response_frame_buffer_len;

            char *payload_data = create_message_json(
                m->matchID,
                (m->playerX == opponent) ? 'X' : 'O',
                "GAME_OVER",
                "Opponent disconnected. You win!",
                &payload_len);

            createFrame(1, 0, 0, 0, 1,
                        payload_len,
                        payload_data,
                        response_frame_buffer,
                        &response_frame_buffer_len);

            send(opponent,
                 response_frame_buffer,
                 response_frame_buffer_len,
                 0);

            free(payload_data);
            printf("Client disconnect response sent to another peer!\n");
        }

        matches[m->matchID] = NULL;
        free(m);
    }
}

void close_connection(int epfd, Client *c)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, c->fd, NULL);
    close(c->fd);
    printf("Client Connection Closed : %d\n", c->fd);
    c->active = 0;
    c->connected = 0;
    free(c);
    return;
}

int main()
{

    for (int i = 0; i < MAX_MATCHES; i++)
    {
        matches[i] = NULL;
    }

    int listen_fd = get_listening_socket();

    int epfd;
    struct epoll_event event[MAX_EVENTS];

    if ((epfd = epoll_create1(0)) == -1)
    {
        die("epoll_create1()");
    }

    Client *listen_client = createClient(listen_fd, NULL);

    add_to_epoll(epfd, listen_client);

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

            Client *c = (Client *)event[i].data.ptr;
            int fd = c->fd;
            uint32_t eve = event[i].events;

            if (eve & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // socket is broken
                close_connection(epfd, c);
                continue;
            }

            if (fd == listen_fd)
            {

                int client_fd;
                if ((client_fd = accept(listen_fd, NULL, NULL)) < 0)
                {
                    continue;
                }

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

                Client *c = createClient(client_fd, NULL);

                add_to_epoll(epfd, c);

                printf("New Client Connected : %d\n", client_fd);
            }
            else
            {

                char frameBuffer[MAX_BUF_SIZE];
                int n;

                if ((n = recv(fd, frameBuffer, MAX_BUF_SIZE, 0)) <= 0)
                {
                    close_connection(epfd, c);
                    continue;
                }

                int opcode;
                Frame *recvframe = parse_frame(frameBuffer, n, &opcode);
                printFrame(recvframe);
                if (opcode == 8)
                {
                    printf("Client Closed Connection!\n");

                    if (c->currentMatch != NULL)
                    {
                        send_peer_close_response_to_another_peer(c->currentMatch, fd);
                        c->currentMatch = NULL;
                    }

                    close_connection(epfd, c);
                    free(recvframe);
                    continue;
                }

                Message *recvmessage = parse_json_string(recvframe->payload_data, recvframe->payload_len);
                printMessage(recvmessage);

                if (strcmp("CREATE_MATCH", recvmessage->code) == 0)
                {
                    Match *newMatch = createMatch(fd, -1, match_id++);
                    c->currentMatch = newMatch;
                    matches[newMatch->matchID] = newMatch;
                    newMatch->players++;

                    uint8_t response_frame_buffer[1024];
                    int payload_len, response_frame_buffer_len;
                    char *payload_data = create_message_json(newMatch->matchID, 'X', "MATCH_CREATED", " ", &payload_len);
                    printf("%s\n", payload_data);
                    createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                    if (send(fd, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                    {
                        printf("Not sent\n");
                    }

                    free(payload_data);
                }
                else if (strcmp("JOIN_MATCH", recvmessage->code) == 0)
                {
                    int idx = atoi(recvmessage->message);

                    if (matches[idx] != NULL && matches[idx]->playerX != -1 && matches[idx]->playerO == -1 && matches[idx]->players != 2)
                    {
                        matches[idx]->playerO = fd;
                        matches[idx]->players++;
                        c->currentMatch = matches[idx];

                        uint8_t response_frame_buffer[1024];
                        int payload_len, response_frame_buffer_len;
                        char *payload_data = create_message_json(matches[idx]->matchID, 'Y', "MATCH_JOINED", " ", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                        if (send(fd, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        printf("%d joined match : %s\n", fd, recvmessage->matchid);
                        free(payload_data);

                        // initialize board for both players
                        // for X
                        payload_data = create_message_json(0, 'X', "INIT_BOARD", " ", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                        if (send(matches[idx]->playerX, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);

                        // for O
                        payload_data = create_message_json(0, 'Y', "INIT_BOARD", " ", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                        if (send(matches[idx]->playerO, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);
                    }
                    else
                    {
                        uint8_t response_frame_buffer[1024];
                        int payload_len, response_frame_buffer_len;
                        char *payload_data = create_message_json(0, 'Y', "MATCH_JOIN_FAILED", " ", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                        if (send(fd, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        printf("%d failed to join match : %s\n", fd, recvmessage->matchid);
                        free(payload_data);
                    }
                }
                else if (strcmp("MOVE", recvmessage->code) == 0)
                {
                    // client update
                    int receiver = recvmessage->player == 'X' ? c->currentMatch->playerO : c->currentMatch->playerX;

                    uint8_t response_frame_buffer[1024];
                    int payload_len, response_frame_buffer_len;
                    char *payload_data = create_message_json(atoi(recvmessage->matchid), recvmessage->player, recvmessage->code, recvmessage->message, &payload_len);
                    printf("%s\n", payload_data);
                    createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);

                    if (send(receiver, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                    {
                        printf("Not sent\n");
                    }
                    free(payload_data);

                    // server update

                    int i = recvmessage->message[0] - '0';
                    int j = recvmessage->message[1] - '0';

                    c->currentMatch->board[i][j] = recvmessage->player;
                    c->currentMatch->moves++;

                    char res = evaluate_board(c->currentMatch->board);

                    printf("Result on evaluation : %c\n", res);

                    if (res != '*')
                    {
                        printf("%c WON\n", res);

                        char win[100];
                        sprintf(win, "Player %c Won!!", res);

                        // response to X
                        response_frame_buffer[1024];
                        payload_len, response_frame_buffer_len;
                        payload_data = create_message_json(atoi(recvmessage->matchid), 'X', "GAME_OVER", win, &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);
                        if (send(c->currentMatch->playerX, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);

                        // response to Y

                        response_frame_buffer[1024];
                        payload_len, response_frame_buffer_len;
                        payload_data = create_message_json(atoi(recvmessage->matchid), 'O', "GAME_OVER", win, &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);
                        if (send(c->currentMatch->playerO, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);

                        matches[c->currentMatch->matchID] = NULL;
                        free(c->currentMatch);

                        continue;
                    }

                    if (c->currentMatch->moves == 9)
                    {
                        printf("%c DRAW\n", res);

                        // response to X
                        response_frame_buffer[1024];
                        payload_len, response_frame_buffer_len;
                        payload_data = create_message_json(atoi(recvmessage->matchid), 'X', "GAME_OVER", "Match Draw", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);
                        if (send(c->currentMatch->playerX, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);

                        // response to Y

                        response_frame_buffer[1024];
                        payload_len, response_frame_buffer_len;
                        payload_data = create_message_json(atoi(recvmessage->matchid), 'O', "GAME_OVER", "Match Draw", &payload_len);
                        printf("%s\n", payload_data);
                        createFrame(1, 0, 0, 0, 1, payload_len, payload_data, response_frame_buffer, &response_frame_buffer_len);
                        if (send(c->currentMatch->playerO, response_frame_buffer, response_frame_buffer_len, 0) < 0)
                        {
                            printf("Not sent\n");
                        }
                        free(payload_data);

                        matches[c->currentMatch->matchID] = NULL;
                        free(c->currentMatch);
                    }
                }
            }
        }
    }

    return 0;
}