/*
 * vim:ts=4:sw=4:sts=4:et:ai:cindent:fdm=marker
 */
#include <stdio.h>

int main(int argc, char **argv) {
    int a=0x42;
    int b=0x58;

    printf("a\t== %02x\nb\t== %02x\na | b\t== %02x\na || b\t== %02x\n",
            a, b, a | b, a || b);
    return 0;
}
