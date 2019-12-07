/*
 * vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
 */

#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int checkprompt(const char *question, const char *rightanswer) {
    char *answer = readline(question);
    int comp = (strcmp(answer, rightanswer) == 0);
    free(answer);
    return comp;
}

int blocktrash(off_t offset, uint64_t count) {
    return 0;
}

int main(int argc, char **argv) {
    const char *thebiganswer = "YesIwantTodOsO";
    char *filename;
    struct stat filestat;
    char s[1024];

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
    snprintf(s, sizeof s, "Are you sure you want to whack device %s, and if so, type: '%s': ", filename, thebiganswer);
    if (!checkprompt(s, thebiganswer)) {
        printf("Coward ;)\n");
        exit(250);
    }
    printf("Doing nasty things with %s\n", filename);
    exit(0);
}
