#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

static inline uint64_t htonll(uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)htonl(x & 0xFFFFFFFFULL) << 32) |
            htonl(x >> 32);
#else
    return x;
#endif
}

static inline uint64_t ntohll(uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)ntohl(x & 0xFFFFFFFFULL) << 32) |
            ntohl(x >> 32);
#else
    return x;
#endif
}


#define MAX_TEXT_SIZE 100

#define CLIENT_BUFFER_SIZE (MAX_TEXT_SIZE + (32) + (1))
#define SERVER_BUFFER_SIZE (CLIENT_BUFFER_SIZE + sizeof(int64_t) * 2)



typedef struct client_data
{

    char message[MAX_TEXT_SIZE];
    char timeStr[32];

} Client_Data;

typedef struct server_data
{

    Client_Data echo;
    int64_t user_count;
    int64_t total_count;

} Server_Data;

void pack_client_data(const Client_Data *data, char *buffer)
{
    snprintf(buffer, CLIENT_BUFFER_SIZE,
             "%-*.*s%-*.*s",
             MAX_TEXT_SIZE, MAX_TEXT_SIZE, data->message,
             32, 32, data->timeStr);
}

void unpack_client_data(Client_Data *data, const char *buffer)
{
    memcpy(data->message, buffer, MAX_TEXT_SIZE);
    data->message[MAX_TEXT_SIZE - 1] = '\0';

    memcpy(data->timeStr, buffer + MAX_TEXT_SIZE, 32);
    data->timeStr[31] = '\0';
}

void pack_server_data(const Server_Data *data, char *buffer)
{

    int64_t uc = htonll(data->user_count);
    int64_t tc = htonll(data->total_count);

    pack_client_data(&data->echo, buffer);
    memcpy(buffer + CLIENT_BUFFER_SIZE, &uc, sizeof(uc));
    memcpy(buffer + CLIENT_BUFFER_SIZE + sizeof(uc), &tc, sizeof(tc));
}

void unpack_server_data(Server_Data *data, const char *buffer)
{
    unpack_client_data(&data->echo, buffer);
    memcpy(&data->user_count, buffer + CLIENT_BUFFER_SIZE, sizeof(int64_t));
    data->user_count = ntohll(data->user_count);
    memcpy(&data->total_count, buffer + CLIENT_BUFFER_SIZE + sizeof(int64_t), sizeof(int64_t));
    data->total_count = ntohll(data->total_count);
}