#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    uint8_t buf[1024000];
    int wstatus;
    pid_t child;
    unsigned int sleepsec = 600;

    memset(buf, 0, sizeof buf);
    child = fork();
    if (child == 0) {
        memset(buf, 0xff, sizeof buf);
        printf("I'm the offspring. Sleeping %i seconds...", sleepsec); fflush(stdout);
        sleep(sleepsec);
        printf("\n"); fflush(stdout);
    } else {
        wait(&wstatus);
        printf("PID %i returned %i\n", child, wstatus); fflush(stdout);
    }
    exit(0);
}
