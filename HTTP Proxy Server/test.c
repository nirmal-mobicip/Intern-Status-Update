#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

typedef struct url_info
{
    char *scheme;
    char *host;
    char *port;
    char *path;
} URL;

static char *normalize_url(const char *scheme, const char *url)
{
    if (strstr(url, "://"))
        return strdup(url);

    char *full = malloc(strlen(scheme) + strlen(url) + 4);
    sprintf(full, "%s://%s", scheme, url);
    return full;
}

int parse_url(const char *scheme, const char *url, URL *out)
{
    CURLU *u = NULL;
    CURLUcode rc;
    char *full_url = NULL;

    memset(out, 0, sizeof(*out));

    full_url = normalize_url(scheme, url);

    u = curl_url();
    if (!u)
        goto fail;

    if (curl_url_set(u, CURLUPART_URL, full_url, 0) != CURLUE_OK)
        goto fail;

    curl_url_get(u, CURLUPART_SCHEME, &out->scheme, 0);
    curl_url_get(u, CURLUPART_HOST, &out->host, 0);

    rc = curl_url_get(u, CURLUPART_PATH, &out->path, 0);
    if (rc != CURLUE_OK)
        out->path = strdup("/");

    rc = curl_url_get(u, CURLUPART_PORT, &out->port, 0);
    if (rc == CURLUE_NO_PORT)
    {
        out->port = strdup(strcmp(out->scheme, "https") == 0 ? "443" : "80");
    }

    curl_url_cleanup(u);
    free(full_url);
    return 0;

fail:
    if (u)
        curl_url_cleanup(u);
    free(full_url);
    return -1;
}

int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    URL url;
    parse_url("http", "https://youtube.com/home", &url);

    printf("scheme: %s\n", url.scheme);
    printf("host:   %s\n", url.host);
    printf("port:   %s\n", url.port);
    printf("path:   %s\n", url.path);

    curl_free(url.scheme);
    curl_free(url.host);
    free(url.port);
    free(url.path);

    curl_global_cleanup();
    return 0;
}
