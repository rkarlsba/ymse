/*
 * vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
 */

#include <errno.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Ugly hardcoding - blame the coder from hell */
const char *thebiganswer = "YesIwantTodOsO";
static char *randsrc = "/dev/urandom";

int checkprompt(const char *question, const char *rightanswer) {
    char *answer;
    int comp;

    answer = readline(question);
    comp = (strcmp(answer, rightanswer) == 0);
    free(answer);

    return comp;
}

// dfd as in destination fd, rfd as in random fd
int blocktrash(int dfd, int rfd, off_t offset, size_t count) {
    void *buf;
    size_t br,bw;

    if (lseek(dfd, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error calling lseek(): %s\n", strerror(errno));
        exit(10);
    }

    buf=malloc(count+1);
    if (buf == NULL) {
        fprintf(stderr, "malloc(%ld) failed\n", count+1);
        exit(11);
    }

    br = read(rfd, buf, count);
    if (br != count) {
        fprintf(stderr, "Read %ld out of %ld bytes, exiting\n", br, count);
        exit(12);
    }

    bw = write(dfd, buf, count);
    if (bw != count) {
        char s[1024] = "";
        if (bw > 0) {
            sprintf(s, " (probably bust anyway)");
        }
        fprintf(stderr, "Read %ld out of %ld bytes, exiting%s\n", br, count, s);
        exit(13);
    }
    return 0;
}

int main(int argc, char **argv) {
    char *devfilename;
    struct stat filestat;
    char s[1024];
    int rand_fd, dev_fd;

    /* Check args, open files etc */
    if (argc != 2) {
        fprintf(stderr, "Syntax: messup <blockdevice>\n");
        exit(1);
    }

    devfilename = argv[1];
    if (stat(devfilename, &filestat) != 0) {
        fprintf(stderr, "stat(\"%s\"): %s\n", devfilename, strerror(errno));
        exit(2);
    }
    if ((filestat.st_mode & S_IFMT) != S_IFBLK) {
        fprintf(stderr, "%s isn't a block device\n", devfilename);
        exit(3);
    }
    snprintf(s, sizeof s, "Are you sure you want to whack device %s, and if so, type: '%s': ", devfilename, thebiganswer);
    if (!checkprompt(s, thebiganswer)) {
        printf("Coward ;)\n");
        exit(250);
    }

    rand_fd = open(randsrc, O_RDONLY);
    if (rand_fd < 0) {
        fprintf(stderr, "Error opening random source '%s': %s\n", randsrc, strerror(errno));
        exit(4);
    }

    dev_fd = open(devfilename, O_RDWR | O_SYNC);
    if (dev_fd < 0) {
        fprintf(stderr, "Error opening device '%s' for reading and writing: %s\n", devfilename, strerror(errno));
        exit(5);
    }

    printf("Doing nasty things with %s\n", devfilename);

//    for (;;) {
        blocktrash(dev_fd, rand_fd, off, cnt);
//    }

    close(dev_fd);
    close(rand_fd);

    printf("Done\n");

    exit(0);
}








