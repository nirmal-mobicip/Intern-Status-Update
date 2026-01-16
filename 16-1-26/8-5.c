// Exercise 8-5. Modify the fsize program to print the other information contained in the inode
// entry.


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <inttypes.h>

#define MAX_PATH 1024

char *trim_ctime(time_t *t)
{
    char *s = ctime(t);
    s[strlen(s) - 1] = '\0';  // remove trailing newline
    return s;
}


void my_dirwalk(char *dir, void (*fcn)(char *))
{
    char name[MAX_PATH];
    struct dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(dir)) == NULL)
    {
        fprintf(stderr, "dirwalk: can't open %s\n", dir);
        return;
    }
    while ((dp = readdir(dfd)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..")==0)
            continue;
        /* skip self and parent */
        if (strlen(dir) + strlen(dp->d_name) + 2 > sizeof(name))
            fprintf(stderr, "dirwalk: name %s %s too long\n",
                    dir, dp->d_name);
        else
        {
            sprintf(name, "%s/%s", dir, dp->d_name);
            (*fcn)(name);
        }
    }
    closedir(dfd);
}

void my_fsize(char *name)
{
    struct stat stbuf;

    if (stat(name, &stbuf) == -1)
    {
        fprintf(stderr, "fsize: can't access %s\n", name);
        return;
    }
    if ((stbuf.st_mode & __S_IFMT) == __S_IFDIR)
        my_dirwalk(name, my_fsize);
    
    printf("%-20s %10ld %-24s %-24s %-24s %12ju\n",
       name,
       stbuf.st_size,
       trim_ctime(&stbuf.st_ctime),
       trim_ctime(&stbuf.st_atime),
       trim_ctime(&stbuf.st_mtime),
       (uintmax_t)stbuf.st_ino);

}



int main(int argc, char **argv)
{
    printf("%-20s %10s %-24s %-24s %-24s %12s\n",
       "Name",
       "Size",
       "Status Changed",
       "Accessed",
       "Modified",
       "Inode");
    printf("%-20s %10s %-24s %-24s %-24s %12s\n",
       "--------------------",
       "----------",
       "------------------------",
       "------------------------",
       "------------------------",
       "------------");
    if (argc == 1) /* default: current directory */
        my_fsize(".");
    else
        while (--argc > 0)
            my_fsize(*++argv);
    return 0;
}