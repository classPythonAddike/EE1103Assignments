#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int base = atoi(argv[1]), power = atoi(argv[2]);

    if (base == 2) {
        printf("%llu\n", ((unsigned long long int)1) << power);
        return 0;
    }

    if (power == 1) {
        printf("%d", base);
        return 0;
    }

    int i = 1;
    unsigned long long int exponent = base;

    while (i << 1 <= power) {
        exponent *= exponent;
        i <<= 1;
    }

    while (i < power) {
        exponent *= base;
        i++;
    }

    printf("%llu\n", exponent);
}
