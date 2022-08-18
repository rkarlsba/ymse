#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

char test1[1<<23]; // 8MB
uint64_t test2[1<<20]; // 8MB

int main(int argc, char **argv) {
    printf("Size of test1: %ld\n", sizeof test1);
    printf("Size of test2: %ld\n", sizeof test2);

    strncpy((char *)test2, test1, sizeof test1);
    return 0;
}
