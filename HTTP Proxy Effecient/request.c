#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct
{
    char *url;
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

char* getKeyfromRequest(Request* req){
    char* key = malloc(100*sizeof(char));
    memset(key,0,100);
    strcat(key, req->method);
    strcat(key, req->url.host);
    strcat(key, req->url.path);
    return key;
}

char *normalize_url(const char *scheme, const char *url)
{
    if (strstr(url, "://"))
        return strdup(url);

    char *full = malloc(strlen(scheme) + strlen(url) + 4);
    sprintf(full, "%s://%s", scheme, url);
    return full;
}
int parse_url(const char *scheme, const char *url, URL *out)
{
    char *full = normalize_url(scheme, url);

    if (!full)
    {
        return -1;
    }

    char *p = strstr(full, "://");

    if (!p)
    {
        free(full);
        return -1;
    }

    p += 3; // skip ://

    // Store scheme properly
    size_t scheme_len = p - full - 3;
    out->scheme = strndup(full, scheme_len);
    out->url = strdup(full);

    // -------- HOST ----------
    char *host_start = p;
    while (*p && *p != ':' && *p != '/')
        p++;

    size_t host_len = p - host_start;
    out->host = strndup(host_start, host_len);

    // -------- PORT ----------
    if (*p == ':')
    {
        p++;
        char *port_start = p;

        while (*p && *p != '/')
            p++;

        size_t port_len = p - port_start;
        out->port = strndup(port_start, port_len);
    }
    else
    {
        out->port = strdup(
            strcmp(out->scheme, "https") == 0 ? "443" : "80");
    }

    // -------- PATH ----------
    if (*p == '\0')
    {
        out->path = strdup("/");
    }
    else
    {
        out->path = strdup(p);
    }

    free(full);
    return 0;
}

int parse_request(char *buf, Request *req)
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