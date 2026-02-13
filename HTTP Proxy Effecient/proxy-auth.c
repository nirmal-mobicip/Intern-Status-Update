#include <openssl/evp.h>

typedef struct
{
    char username[64], password[64];
} User;

User Authorised_Users[100];
int user_count = 0;

void add_user(const char *username, const char *password)
{
    if (user_count >= 100)
        return;

    strncpy(Authorised_Users[user_count].username, username, 63);
    strncpy(Authorised_Users[user_count].password, password, 63);

    user_count++;
}

int authenticate(const char *username, const char *password)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(Authorised_Users[i].username, username) == 0 &&
            strcmp(Authorised_Users[i].password, password) == 0)
        {
            return 1; // valid
        }
    }
    return 0; // invalid
}

int validate(char *data)
{
    char *username = data;
    char *password;
    while (*data != ':')
    {
        data++;
    }
    *data = '\0';
    data++;
    password = data;
    while (*data != '\0')
    {
        data++;
    }
    *data = '\0';
    return authenticate(username, password);
}

int authorize(int client_fd,char *buf)
{
    char *ptr;
    printf("Client FD : %d\nDATA : \n%s\n",client_fd,buf);
    if ((ptr = strstr(strdup(buf), "Proxy-Authorization: Basic ")))
    {
        ptr += 27;
        char *base64encodedstr = ptr;
        while (*ptr != '\r')
        {
            ptr++;
        }
        *ptr = '\0';
        unsigned char decoded[256];
        int l = EVP_DecodeBlock(decoded, base64encodedstr, strlen(base64encodedstr));
        decoded[l] = '\0';

        if (!validate(decoded))
        {
            char *resp = "HTTP/1.1 407 Proxy Authentication Required\r\nProxy-Authenticate: Basic realm = \"MyProxy\"\r\nConnection : close\r\n";
            send(client_fd, resp, strlen(resp), 0);
            printf("\nProxy Authentication Failed\n");
            return 0;
        }
        else
        {
            printf("\nUser is Proxy Authorized\n");
            return 1;
        }
    }
    else
    {
        char *resp = "HTTP/1.1 407 Proxy Authentication Required\r\nProxy-Authenticate: Basic realm = \"MyProxy\"\r\nConnection : close\r\n";
        send(client_fd, resp, strlen(resp), 0);
        printf("\nProxy Authentication Failed\n");
        return 0;
    }
}