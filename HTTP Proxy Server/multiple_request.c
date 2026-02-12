#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main()
{
    // char *args[] = {
    //     "./http_request",
    //     "-m", "GET",
    //     "-x", "http://localhost:8080",
    //     "-h", "https://wikipedia.org",
    //     "-a", "nirmal:mobicipintern",
    //     NULL};

    char *args[] = {
        "./http_request",
        "-x","http://localhost:8080","-h", "https://wikipedia.org","-a","nirmal:mobicipintern","-m","GET",
        NULL};

    for (int i = 0; i < 100; i++)
    {
        if (fork() == 0)
        {
            execvp(args[0], args);
            _exit(1);
        }
    }

    // Wait for all children
    while (wait(NULL) > 0)
        ;
}