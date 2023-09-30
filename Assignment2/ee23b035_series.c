/*
 * Roll Number: EE23B035
 * Date: 18/08/2023
 * Version:
 * Description: Evaluate first `n` terms of sin(x) expansion, and determine the error.
 * Inputs: Number of terms: n, and Angle: x
 * Outputs: sin(x) calculated from Taylor Series, Error
 * Indentation: 4 spaces
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

long long int factorial(int);
void approx_sin(double, int, double*);

int main(int argc, char** argv) {
    int n = atoi(argv[1]);
    double x = atof(argv[2]), n_sin, error;

    // Approximate sin(x) and compute the error.
    approx_sin(x, n, &n_sin);
    error = sin(x) - n_sin;
    
    printf("%.3f,%.3f\n", n_sin, error);
}

// Return the factorial of a given integer n - uses 64 bit integers to allow for larger numbers to be used.
long long int factorial(int n) {
    if (n > 1) {
        return ((long long int)n) * factorial(n - 1);
    } else {
        return 1;
    }
}

// Approximate sin(x) using the first n terms of the Taylor series.
void approx_sin(double x, int n, double* sin_result) {
    double multiplier = 1.0;

    for (int i = 1; i <= 2 * n + 1; i += 2) {
        *sin_result += multiplier * ((double)powf(x, i)) / (double)factorial(i);
        multiplier *= -1.0;
    }

}
