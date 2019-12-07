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

int blocktrash(int dfd, int rfd, off_t offset, size_t count) {
    if (lseek(dfd, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error calling lseek(): %s\n", strerror(errno));
        exit(10);
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

    /*
    for (;;) {
        blocktrash(dev_fd, rand_fd, off, cnt);
    }
    */

    printf("Doing nasty things with %s\n", devfilename);
    exit(0);
}








