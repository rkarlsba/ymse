/*
 * vim:ts=4:sw=4:sts=4:et:ai:fdm=marker
 */
#include <stdio.h>
#include <string.h>

char s[1024] = "123\n";

int main() {
    int a=0,b=0,c=0;

    a=b=1;
    printf("a is %i, b is %i and c is %i\n", a, b, c);

    printf("String is\n%s", s);
    strncat(s, "mer jall\n", sizeof(s) - strlen(s) - 1);
    printf("And now, string is\n%s", s);
    printf("Test %%f: %%f\n");
    for (int i=0; i<15; i++) {
        printf("%i and %s\n", i, i % 7 ? "nah" : "bingo!");
    }
    return 0;
}
