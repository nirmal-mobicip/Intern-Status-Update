#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#undef BUFSIZ
#define BUFSIZ 1024
#define OPEN_MAX 20
#define PERMS 0666

#define my_getc(p)                    \
    (--(p)->cnt >= 0                  \
         ? (unsigned char)*(p)->ptr++ \
         : _my_fillbuf(p))

#define my_putc(x, p) \
    (--(p)->cnt >= 0 ? *(p)->ptr++ = (x) : _my_flushbuf((x), p))

enum _flags
{
    _READ = 01,
    _WRITE = 02,
    _UNBUF = 04,
    _EOF = 010,
    _ERR = 020
};

typedef struct _iobuf
{
    int cnt;    /* characters left in buffer */
    char *ptr;  /* next character position */
    char *base; /* pointer to buffer */
    // int flag;    /* mode flags: read/write/error/eof */

    unsigned int _read : 1;
    unsigned int _write : 1;
    unsigned int _unbuf : 1;
    unsigned int _eof : 1;
    unsigned int _err : 1;

    int fd; /* file descriptor */
} MY_FILE;

MY_FILE _iob[OPEN_MAX] = {
    {0, NULL, NULL, 1, 0, 0, 0, 0, 0}, // stdin
    {0, NULL, NULL, 0, 1, 0, 0, 0, 1}, // stdout
    {0, NULL, NULL, 0, 1, 1, 0, 0, 2}, // stderr (unbuffered)
};

MY_FILE *my_fopen(char *name, char *mode)
{
    int fd;
    MY_FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a')
        return NULL;

    for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
        if (fp->_read == 0 && fp->_write == 0)
            break; /* found a free slot */
    if (fp >= _iob + OPEN_MAX)
        return NULL; /* no free slot */

    fp->_read = fp->_write = fp->_unbuf = fp->_eof = fp->_err = 0;
    fp->ptr = NULL;
    fp->cnt = 0;

    if (*mode == 'w')
        fd = creat(name, PERMS);
    else if (*mode == 'a')
    {
        if ((fd = open(name, O_WRONLY, 0)) == -1)
            fd = creat(name, PERMS);
        lseek(fd, 0L, 2); /* seek to end */
    }
    else
    {
        fd = open(name, O_RDONLY, 0);
    }

    if (fd == -1)
        return NULL; /* open failed */

    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL; /* buffer created later */
    if (*mode == 'r')
    {
        fp->_read = 1;
    }
    else
    {
        fp->_write = 1;
    }
    return fp;
}

int _my_flushbuf(int c, MY_FILE *fp)
{
    if (fp->_write != 1 || fp->_err == 1)
    {
        return EOF;
    }

    int bufsize;
    bufsize = (fp->_unbuf) ? 1 : BUFSIZ;

    if (fp->_unbuf)
    {
        if (write(fp->fd, &c, 1) != 1)
        {
            fp->_err = 1;
            return EOF;
        }
        return c;
    }

    if (fp->base == NULL)
    { /* allocate buffer */
        if ((fp->base = malloc(bufsize)) == NULL)
        {
            fp->_err = 1;
            return EOF;
        }
        fp->ptr = fp->base;
        fp->cnt = bufsize;
    }

    int n = fp->ptr - fp->base;
    if (n > 0)
    {
        if (write(fp->fd, fp->base, n) != n)
        {
            fp->_err = 1;
            return EOF;
        }
    }

    fp->cnt = bufsize - 1;
    fp->ptr = fp->base;

    *fp->ptr++ = c;

    return c;
}

int _my_fillbuf(MY_FILE *fp)
{
    int bufsize;

    if (fp->_read != 1 || fp->_eof == 1 || fp->_err == 1)
    {
        return EOF;
    }

    bufsize = (fp->_unbuf) ? 1 : BUFSIZ;

    if (fp->base == NULL) /* allocate buffer */
        if ((fp->base = (char *)malloc(bufsize)) == NULL)
            return EOF;

    fp->ptr = fp->base;
    fp->cnt = read(fp->fd, fp->ptr, bufsize);

    if (fp->cnt <= 0)
    {
        if (fp->cnt == 0) /* no chars read = EOF */
            fp->_eof = 1;
        else
            fp->_err = 1;
        fp->cnt = 0;
        return EOF;
    }
    fp->cnt--;
    return (unsigned char)*fp->ptr++;
}

int my_fclose(MY_FILE *fp)
{
    if (fp == NULL)
        return EOF;

    if (fp->_write == 1 && fp->base != NULL && fp->ptr > fp->base)
    {
        int n = fp->ptr - fp->base;
        if (write(fp->fd, fp->base, n) != n)
        {
            fp->_err = 1;
            return EOF;
        }
    }

    if (fp->base != NULL)
    {
        free(fp->base);
        fp->base = NULL;
    }

    if (fp->fd >= 0)
    {
        close(fp->fd);
        fp->fd = -1;
    }

    fp->_read = 0;
    fp->_write = 0;
    fp->_unbuf = 0;
    fp->_eof = 0;
    fp->_err = 0;

    fp->ptr = NULL;
    fp->cnt = 0;

    return 0;
}

int my_fflush(MY_FILE *fp)
{
    if (fp == NULL)
        return 0;

    if (fp->_write != 1 || fp->_err == 1)
        return EOF;

    if (fp->_unbuf)
        return 0;

    if (fp->base != NULL && fp->ptr > fp->base)
    {
        int n = fp->ptr - fp->base;
        if (write(fp->fd, fp->base, n) != n)
        {
            fp->_err = 1;
            return EOF;
        }
    }

    fp->ptr = fp->base;
    fp->cnt = BUFSIZ;

    return 0;
}


int main()
{

    // write

    MY_FILE *fp = my_fopen("file.txt", "w");
    if (fp == NULL)
    {
        printf("Error\n");
        return 0;
    }
    char *str = "hello everyone\nHow are you\nwelcome to mobicip\n";

    for (int i = 0; i < strlen(str); i++)
    {
        my_putc(str[i], fp);
    }
    my_fflush(fp);    // before write mode closes call flush 
    my_fclose(fp);

    // read
    fp = my_fopen("file.txt", "r");
    if (fp == NULL)
    {
        printf("Error\n");
        return 0;
    }

    int c;
    while ((c = my_getc(fp)) != EOF)
    {
        printf("%c", c);
    }

    
    my_fclose(fp);

    return 0;
}