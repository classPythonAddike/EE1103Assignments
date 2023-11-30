/*
 * Roll Number: EE23B035
 * Date: 23/10/2023
 * Description: Given a range of x, order `n` and x-coordinate `x_0`, generate cubic splines to interpolate f(x) and predict the value of f(x) at `x_0`.
 * Input:
 *  - Argument: Order of Polynomial (int)
 *  - Argument: x_0 (long double) - Point to estimate f(x) at
 *  - Argument: x_start (long double) - Start of range to interpolate f(x)
 *  - Argument: x_end (long double) - End of range to interpolate f(x)
 * Output: Approximation of f(x_0) using the cubic splines.
 * Indentation: 4 spaces
 */

#define TOL 1e-8 // Tolerance to classify a number as 0 or not
#define STATUS_OK 0
#define STATUS_ERROR 2

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Struct to hold (x, y) coordinates
struct coordinate {
    long double x;
    long double y;
};

long double *eqns; // Coefficients for each equation to solve for coefficients of the cubic splines
long double *solutions; // Coefficients for the cubic splines
struct coordinate *data_points; // Sampled data

long double f(long double); // Function to interpolate
void solve(int); // Solve the given system of equations and write the solutions to the solutions array
void pivot(int, int); // Pivot two rows so that the maximum appears on the diagonal
void eliminate(int, int); // Eliminate the coefficients in a given column
void condition_3(int, int); // Condition 3: First derivative continuous at interior points
void condition_4(int, int); // Condition 4: Second derivative continuous at interior points
void condition_5(int, int); // Condition 5: Second derivative = 0 at end points
void swap_rows(int, int, int); // Swap two rows given the row numbers
long double substitute_x_0(long double, int); // Approximate f(x_0) using the cubic spline
void normalise_coefficients(int); // num_eqnsormalise each row so that the maximum modulus value of a coefficient is 1
void condition_1_and_2(int, int); // Condition 1: Continuity at interior points, Condition 2: Pass through end points
void subtract_rows(int, int, long double, int); // Subtract each element in row 2 multiplied by a factor from the corresponding element in row 1

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Please provide arguments in the following order: order x_0 x_start x_end\n");
        exit(STATUS_ERROR);
    }
    
    int num_points = atoi(argv[1]) + 1;
    long double x_start = atof(argv[3]), x_end = atof(argv[4]), x_0 = atof(argv[2]);

    int num_cols = 4 * num_points - 3;
    int num_eqns = num_cols - 1;
    
    // Eqns are formatted like
    // a0 * A00 + a1 * A01 + a2 * A02 ... + b0 * B00 + b1 * B01 ... + c0 * C00 ... + d0 * D00 ... = K0
    // a0 * A10 + a1 * A11 + a2 * A12 ... + b0 * B10 + b1 * B11 ... + c0 * C10 ... + d0 * D10 ... = K1
    // a0 * A20 + a1 * A21 + a2 * A22 ... + b0 * B20 + b1 * B21 ... + c0 * C20 ... + d0 * D20 ... = K2
    // ....
    eqns = (long double*)calloc(num_eqns * num_cols, sizeof(long double));
    solutions = (long double*)calloc(num_eqns, sizeof(long double));

    // Allocate space for the points in memory
    data_points = (struct coordinate*)malloc(num_points * sizeof(struct coordinate));
    // Take equally spaced points to perform the lagrangian on
    for (int i = 0; i < num_points; i++) {
        data_points[i].x = (x_start * (num_points - 1 - i) + x_end * i) / (num_points - 1);
        data_points[i].y = f(data_points[i].x);
    }

    // For n points: 4 * (n - 1) variables
    // Condition 1: Continuity at interior points -------------------> 2 * (n - 2) = 2n - 4
    // Condition 2: Pass through end points -------------------------> 2
    // Condition 3: First derivative continuous at interior points --> 2 * (n - 2) = n - 2
    // Condition 4: Second derivative continuous at interior points -> 2 * (n - 2) = n - 2
    // Condition 5: Second derivative = 0 at end points -------------> 2
    // -------------------------------------------------------------------------------------
    // Total --------------------------------------------------------> 4n - 4 equations
    
    condition_1_and_2(0, num_points);
    condition_3(num_cols * (2 * (num_points - 2) + 2), num_points);
    condition_4(num_cols * (3 * (num_points - 2) + 2), num_points);
    condition_5(num_cols * (4 * (num_points - 2) + 2), num_points);

    // normalise_coefficients(num_eqns);
    solve(num_eqns);

    printf("%Lf\n", substitute_x_0(x_0, num_points));
    
    free(eqns);
    free(solutions);
    free(data_points);
}

long double f(long double x) {
    return 1.0 / (1.0 + 25.0 * x * x);
}

// Condition 1: Continuity at interior points, Condition 2: Pass through end points
void condition_1_and_2(int eqns_index, int num_points) {
    int num_eqns = 4 * (num_points - 1);
    int num_cols = num_eqns + 1;

    // Pointer to start of equation in the array of equations
    long double *eqns_start = eqns + eqns_index;
    
    for (int eqn = 1; eqn < num_points; eqn++) {
        for (int point_nr = 0; point_nr < 2; point_nr++) {
            long double x = data_points[eqn + point_nr - 1].x;

            for (int i = 0; i < num_cols - 1; i++) {
                if (i % (num_points - 1) == eqn - 1) {
                    // General coefficient = x_i^n
                    eqns_start[i] = powf(x, (long double)(3.0 - (float)i / (num_points - 1)));
                }
            }
            
            // Constant is the y-coordinate
            eqns_start[num_cols - 1] = data_points[eqn + point_nr - 1].y;
            eqns_start += num_cols;
        }
    }
}

// Condition 3: First derivative continuous at interior points
void condition_3(int eqns_index, int num_points) {
    int num_eqns = 4 * (num_points - 1);
    int num_cols = num_eqns + 1;

    // Pointer to start of equation in the array of equations
    long double *eqns_start = eqns + eqns_index;

    for (int point = 1; point < num_points - 1; point++) {
        long double x = data_points[point].x;
        long double y = data_points[point].y;

        for (int i = 0; i < 3 * (num_points - 1); i++) {
            if (i % (num_points - 1) == point - 1) {
                // General coefficient = -1 * x_i^(n - 1) * n
                eqns_start[i] = -1.0 * powf(x, (long double)(2.0 - (float)i / (num_points - 1))) * (long double)(3.0 - (float)i / (num_points - 1));
                // General coefficient = x_i^(n - 1) * n
                eqns_start[i + 1] = -1.0 * eqns_start[i];
                i++;
            }
        }
        
        eqns_start += num_cols;
    }
}

// Condition 4: Second derivative continuous at interior points
void condition_4(int eqns_index, int num_points) {
    int num_eqns = 4 * (num_points - 1);
    int num_cols = num_eqns + 1;

    // Pointer to start of equation in the array of equations
    long double *eqns_start = eqns + eqns_index;
    
    for (int point = 1; point < num_points - 1; point++) {
        long double x = data_points[point].x;
        long double y = data_points[point].y;

        for (int i = 0; i < 2 * (num_points - 1); i++) {
            if (i % (num_points - 1) == point - 1) {
                // General coefficient = -1 * x_i^(n - 2) * n * (n - 1)
                eqns_start[i] = -1.0 * powf(x, (long double)(1.0 - (float)i / (num_points - 1))) * (long double)(3.0 - (float)i / (num_points - 1)) * (long double)(2.0 - (float)i / (num_points - 1));
                // General coefficient = x_i^(n - 2) * n * (n - 1)
                eqns_start[i + 1] = -1.0 * eqns_start[i];
                i++;
            }
        }
        
        eqns_start += num_cols;
    }
}

// Condition 5: Second derivative = 0 at end points
void condition_5(int eqns_index, int num_points) {
    int num_eqns = 4 * (num_points - 1);
    int num_cols = num_eqns + 1;

    // Pointer to start of equation in the array of equations
    long double *eqns_start = eqns + eqns_index;

    eqns_start[0] = 6.0 * data_points[0].x;
    eqns_start[num_points - 1] = 2.0;

    eqns_start += num_cols;
    eqns_start[0] = 6.0 * data_points[num_points - 1].x;
    eqns_start[num_points - 1] = 2.0;
}

// Divide each coefficient by the maximum coefficient in the row to reduce round-off errors
void normalise_coefficients(int num_rows) {
    long double row_max;
    for (int row = 0; row < num_rows; row++) {
        row_max = fabsl(eqns[row * (num_rows + 1)]);
        for (int column = 1; column < num_rows + 1; column++) {
            row_max = row_max > fabsl(eqns[row * (num_rows + 1) + column]) ? row_max : fabsl(eqns[row * (num_rows + 1) + column]);
        }

        for (int column = 0; column < num_rows + 1; column++) {
            eqns[row * (num_rows + 1) + column] /= row_max;
        }
    }


}

// Given the row numbers of two rows, swap them
void swap_rows(int row1, int row2, int num_rows) {
    long double *row1_ptr = eqns + row1 * (num_rows + 1), *row2_ptr = eqns + row2 * (num_rows + 1), temp;
    for (int column = 0; column < num_rows + 1; column++) {
        temp = row1_ptr[column];
        row1_ptr[column] = row2_ptr[column];
        row2_ptr[column] = temp;
    }
}

// Pivot a row so that the number on the diagonal has the maximum value
void pivot(int row, int num_rows) {
    int max_row = row;
    int column = row;

    for (int temp_row = row; temp_row < num_rows; temp_row++) {
        max_row = fabsl(eqns[max_row * (num_rows + 1) + column]) < fabsl(eqns[temp_row * (num_rows + 1) + column]) ? temp_row : max_row;
    }

    swap_rows(max_row, row, num_rows);
}

// Subtract the elements of the second row times a multiplier from the first row's corresponding element
void subtract_rows(int row1, int row2, long double multiplier, int num_rows) {
    for (int column = 0; column < num_rows + 1; column++) {
        eqns[row1 * (num_rows + 1) + column] -= multiplier * eqns[row2 * (num_rows + 1) + column];
    }
}

// Eliminate the eqns in a given column to generate the upper triangular matrix
void eliminate(int column, int num_rows) {
    int pivot_row = column;
    pivot(pivot_row, num_rows);

    for (int row = pivot_row + 1; row < num_rows; row++) {
        // If the element is not 0, then make it 0
        if (fabsl(eqns[row * (num_rows + 1) + column]) > TOL) {
            subtract_rows(row, pivot_row , eqns[row * (num_rows + 1) + column] / eqns[pivot_row * (num_rows + 1) + column], num_rows);
        }
    }
}

// Solve the given system of equations
void solve(int num_rows) {
    // Generate the upper triangular matrix
    for (int column = 0; column < num_rows - 1; column++) {
        eliminate(column, num_rows);
    }
    
    // Back Substitution
    long double matrix_multiplication;
    for (int var = num_rows - 1; var >= 0; var--) {
        matrix_multiplication = eqns[var * (num_rows + 1) + num_rows];
        for (int column = var + 1; column < num_rows; column++) {
            matrix_multiplication -= eqns[var * (num_rows + 1) + column] * solutions[column];
        }

        // If an element on the diagonal is 0, then the equations have infinite solutions
        if (fabsl(eqns[var * (num_rows + 1) + var]) <= TOL) {
            fprintf(stderr, "This system of equations cannot be solved!\n");
            exit(STATUS_ERROR);
        }

        solutions[var] = matrix_multiplication / eqns[var * (num_rows + 1) + var];
    }
}

// Approximate f(x_0) using the cubic splines
long double substitute_x_0(long double x_0, int num_points) {
    int f_number = 0;

    // Find out which spline to use to approximate
    if (x_0 <= data_points[0].x) {
        f_number = 1;
    } else if (x_0 >= data_points[num_points - 1].x) {
        f_number = num_points;
    } else {
        for (f_number = 1; f_number < num_points; f_number++) {
            if (x_0 > data_points[f_number - 1].x) {
                break;
            }
        }
    }

    long double f_x_0 = 0;
    // Calculate f(x_0) using the spline
    for (int i = 0; i < 4; i++) {
        f_x_0 += powf(x_0, 3 - i) * solutions[f_number - 1 + (num_points - 1) * i];
    }

    return f_x_0;
}
