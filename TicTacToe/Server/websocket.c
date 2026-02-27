// openssl
#include <openssl/sha.h>
#include <openssl/evp.h>

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