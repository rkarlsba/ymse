/*
 * vim:ts=4:sw=4:sts=4:et:ai:cindent:fdm=marker
 */
#include <stdio.h>

int main(int argc, char **argv) {
    int a=0x01;
    int b=0x02;

    printf("a\t== %02x\nb\t== %02x\na | b\t== %02x\na || b\t== %02x\n",
            a, b, a | b, a || b);
    a ? printf("a is true\n") : printf("a is false\n");
    b ? printf("b is true\n") : printf("b is false\n");
    (a|b) ? printf("(a|b) is true\n") : printf("(a|b) is false\n");
    (a||b) ? printf("(a||b) is true\n") : printf("(a||b) is false\n");
    return 0;
}
