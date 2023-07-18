// vim:ts=4:sw=4:sts=4:et:ai:fdm=marker

#include <stdio.h>

int main(void) {
    double numbers[] = {0, [10]=55, 89, [5]=5, 8, [1]=1, 1, 2};
    /* innhold {{{
     *
     * [0]:		0.000000
     * [1]:		1.000000
     * [2]:		1.000000
     * [3]:		2.000000
     * [4]:		0.000000
     * [5]:		5.000000
     * [6]:		8.000000
     * [7]:		0.000000
     * [8]:		0.000000
     * [9]:		0.000000
     * [10]:	55.000000
     * [11]:	89.000000
     *
     * }}} */
    int sizeof_double = sizeof(double);
    int sizeof_numbers = sizeof(numbers);
    int elements_in_numbers = sizeof_numbers / sizeof_double;

    printf("sizeof numbers = %i\n", sizeof_numbers);
    printf("sizeof double = %i\n", sizeof_double);
    printf("elements in numbers = %i\n", elements_in_numbers);
    for (int i=0;i<elements_in_numbers;i++) {
        printf("[%i]:\t%f\n", i, numbers[i]);
    }

    return 0;
}
