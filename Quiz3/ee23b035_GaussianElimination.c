/*
 * Roll Number: EE23B035
 * Date: 13/10/2023
 * Version: 1.0.0
 * Description: Given N linear equations, solve them using Gaussian Elimination
 * Input:
 *  - Argument filename: File containing system of equations.
 * Output: Solutions for N variables, or error if the system has no/infinite solutions
 * Indentation: 4 spaces
 */

#define TOL 1e-4 // Tolerance to classify a number as 0 or not
#define STATUS_OK 0
#define STATUS_ERROR 2

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int N = 0; // Number of equations
float *coefficients; // 1D array containing N * (N + 1) coefficients
float *solutions; // Array containing N solutions to the equations

void solve(); // Solve the given system of equations and write the solutions to the solutions array
void pivot(int); // Pivot two rows so that the maximum appears on the diagonal
void eliminate(int); // Eliminate the coefficients in a given column
void count_N(char*); // Find out the number of equations by reading the data file
void swap_rows(int, int); // Swap two rows given the row numbers
void normalise_coefficients(); // Normalise each row so that the maximum modulus value of a coefficient is 1
void read_coefficients(char*); // Read the coefficients from the data file
void subtract_rows(int, int, float); // Subtract each element in row 2 multiplied by a factor from the corresponding element in row 1

int main(int argc, char** argv) {
    count_N(argv[1]);
    coefficients = (float*)malloc(N * (N + 1) * sizeof(float));
    solutions = (float*)calloc(N, sizeof(float));
    
    read_coefficients(argv[1]);
    normalise_coefficients();
    solve();

        for (int i = 0; i < N; i++) {
        printf("%f ", solutions[i]);
    }
    printf("\n");

    free(solutions);
    free(coefficients);

    return STATUS_OK;
}

// Find N from the data file
void count_N(char* file) {
    FILE *f_ptr = fopen(file, "r");
    
    // Find number of lines
    for (char c = getc(f_ptr); c != EOF; c = getc(f_ptr)) {
        if (c == '\n') {
            N += 1;
        }
    }
    fclose(f_ptr);
}

void read_coefficients(char* file) {
    FILE *f_ptr = fopen(file, "r");
    int num_coefficients = 0, return_code;
    for (int row = 0; row < N; row++) {
        for (int column = 0; column < N + 1; column++) {
            return_code = fscanf(f_ptr, "%f", coefficients + row * (N + 1) + column);
            num_coefficients += (return_code != -1); // If the return code is not -1, increment the number of coefficients
        }
    }
    
    fclose(f_ptr);
    
    // Ensure that we read N * (N + 1) coefficients
    if (num_coefficients != N * (N + 1)) {
        fprintf(stderr, "Number of coefficients is not equal to N * (N + 1)!\n");
        exit(STATUS_ERROR);
    }
}

// Divide each coefficient by the maximum coefficient in the row to reduce round-off errors
void normalise_coefficients() {
    float row_max;
    for (int row = 0; row < N; row++) {
        row_max = fabsf(coefficients[row * (N + 1)]);
        for (int column = 1; column < N + 1; column++) {
            row_max = row_max > fabsf(coefficients[row * (N + 1) + column]) ? row_max : fabsf(coefficients[row * (N + 1) + column]);
        }

        for (int column = 0; column < N + 1; column++) {
            coefficients[row * (N + 1) + column] /= row_max;
        }
    }


}

// Given the row numbers of two rows, swap them
void swap_rows(int row1, int row2) {
    float *row1_ptr = coefficients + row1 * (N + 1), *row2_ptr = coefficients + row2 * (N + 1), temp;
    for (int column = 0; column < N + 1; column++) {
        temp = row1_ptr[column];
        row1_ptr[column] = row2_ptr[column];
        row2_ptr[column] = temp;
    }
}

// Pivot a row so that the number on the diagonal has the maximum value
void pivot(int row) {
    int max_row = row;
    int column = row;

    for (int temp_row = row; temp_row < N; temp_row++) {
        max_row = fabsf(coefficients[max_row * (N + 1) + column]) < fabsf(coefficients[temp_row * (N + 1) + column]) ? temp_row : max_row;
    }

    swap_rows(max_row, row);
}

// Subtract the elements of the second row times a multiplier from the first row's corresponding element
void subtract_rows(int row1, int row2, float multiplier) {
    for (int column = 0; column < N + 1; column++) {
        coefficients[row1 * (N + 1) + column] -= multiplier * coefficients[row2 * (N + 1) + column];
    }
}

// Eliminate the coefficients in a given column to generate the upper triangular matrix
void eliminate(int column) {
    int pivot_row = column;
    pivot(pivot_row);

    for (int row = pivot_row + 1; row < N; row++) {
        // If the element is not 0, then make it 0
        if (fabsf(coefficients[row * (N + 1) + column]) > TOL) {
            subtract_rows(row, pivot_row , coefficients[row * (N + 1) + column] / coefficients[pivot_row * (N + 1) + column]);
        }
    }
}

// Solve the given system of equations
void solve() {
    // Generate the upper triangular matrix
    for (int column = 0; column < N - 1; column++) {
        eliminate(column);
    }
    
    // Back Substitution
    float matrix_multiplication;
    for (int var = N - 1; var >= 0; var--) {
        matrix_multiplication = coefficients[var * (N + 1) + N];
        for (int column = var + 1; column < N; column++) {
            matrix_multiplication -= coefficients[var * (N + 1) + column] * solutions[column];
        }
        
        // If an element on the diagonal is 0, then the equations have infinite solutions
        if (coefficients[var * (N + 1) + var] <= TOL) {
            fprintf(stderr, "This system of equations cannot be solved!\n");
            exit(STATUS_ERROR);
        }

        

        solutions[var] = matrix_multiplication / coefficients[var * (N + 1) + var];
    }
}
