#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE (1024*1024)
// #define DEBUG
#define SCANFOR 0

int main(int argc, char *argv[]) {
    int i,j,fd,readmore;
    char buf[BUFSIZE];
    ssize_t bytes_read;
    int foundinblock = 0;
    int verbose = 1;

    if (argc == 1) {
        fprintf(stderr, "Syntax: %s <filename> [ <filename> [  ... ]]\n", argv[0]);
        return 1;
    }

    for (i=1;i<argc;i++) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "Can't open file '%s': %s\n", argv[i], strerror(errno));
            return 1;
        }
        readmore = 1;
        while (readmore && !foundinblock) {
            foundinblock = 0;
            // ssize_t read(int fd, void buf[.count], size_t count);
            bytes_read = read(fd, buf, BUFSIZE);
            if (bytes_read == -1) {
                fprintf(stderr, "Can't read from file '%s': %s\n", argv[i], strerror(errno));
                return 1;
            } else if (bytes_read < BUFSIZE) {
                readmore = 0;
            }
            for (j=0;j<bytes_read;j++) {
                if (buf[j] != SCANFOR) {
                    foundinblock++;
                }
            }
            if (foundinblock > 0 && verbose > 0) {
                printf("Non-null character found in %i occurences in file '%s'\n",
                        foundinblock, argv[i]);
            }
        }
        if (foundinblock == 0) {
            printf("%s: This file is full of nulls\n", argv[i]);
        }
    }

#ifdef DEBUG
    printf("DEBUG: Read %li bytes from file '%s'\n", bytes_read, argv[i]);
#endif // DEBUG
    return 0;
}
