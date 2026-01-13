#include <stdio.h>

void print_file(FILE *ip, FILE *op) {
    char buffer[BUFSIZ];
    size_t n;

    while ((n = fread(buffer, 1, BUFSIZ, ip)) > 0) {
        fwrite(buffer, 1, n, op);
    }
}

int main(int argc, char *argv[]) {
    FILE *fp;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 file2 ...\n", argv[0]);
        return 1;
    }

    while (--argc > 0) {
        fp = fopen(*++argv, "r");
        if (fp == NULL) {
            perror(*argv);
            continue;
        }

        print_file(fp, stdout);
        fclose(fp);
    }

    return 0;
}
