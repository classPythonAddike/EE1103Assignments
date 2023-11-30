/*
 * Roll Number: EE23B035
 * Date: 14/11/2023
 * Description: Given the term number, output the corresponding term of the Hemachandra series.
 * Input:
 *  - Argument: M - M'th term to print
 * Output: M'th term in Hemachandra Series
 * Indentation: 4 spaces
 */

#include <stdio.h>
#include <stdlib.h>

long long hem(long long, long long, long long);

int main(int argc, char **argv) {
    long long M = atoi(argv[1]);
    printf("%lld\n", hem(M, 0, 1));
}

// Recursive function to calculate the N'th term in the Hemachandra series.
long long hem(long long N, long long a, long long b) {
    if (N == 1) {
        return a;
    }
    return hem(N - 1, b, a + b);
}
