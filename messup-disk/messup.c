/*
 * vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// int stat(const char *pathname, struct stat *statbuf);

int main(int argc, char **argv) {
    char *filename;
    struct stat filestat;

    if (argc != 2) {
        fprintf(stderr, "Syntax: messup <blockdevice>\n");
        exit(1);
    }

    filename = argv[1];
    if (stat(filename, &filestat) != 0) {
        fprintf(stderr, "stat(\"%s\"): %s\n", filename, strerror(errno));
        exit(2);
    }
    if ((filestat.st_mode & S_IFMT) != S_IFBLK) {
        fprintf(stderr, "%s isn't a block device\n", filename);
        exit(3);
    }
    printf("Doing nasty things with %s\n", filename);
    exit(0);
}
