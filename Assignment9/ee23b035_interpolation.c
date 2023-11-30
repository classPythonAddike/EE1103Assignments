/*
 * Roll Number: EE23B035
 * Date: 20/10/2023
 * Description: Given a range of x, order `n` and x-coordinate `x_0`, interpolate f(x) using Lagrange's Interpolation, sample `n + 1` points, and predict the value of f(x) at `x_0` using Newton's Interpolation.
 * Input:
 *  - Argument: Order of Polynomial (int)
 *  - Argument: x_start (float) - Start of range to interpolate f(x)
 *  - Argument: x_end (float) - End of range to interpolate f(x)
 *  - Argument: x_0 (float) - Point to estimate f(x) at
 * Output: Newton's Interpolation of f(x) at x = x_0
 * Indentation: 4 spaces
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Struct to hold (x, y) coordinates
struct coordinate {
    float x;
    float y;
};


float f(float); // Function to interpolate
void sample_points_lagrange(int, struct coordinate*); // Sample points from the function using the Lagrange Interpolation
float newton_interpolation(int, struct coordinate*, float);
float lagrange_interpolation(int, struct coordinate*, float);


int main(int argc, char** argv) {
    int order = atoi(argv[1]);
    float x_start = atof(argv[2]), x_end = atof(argv[3]), x_0 = atof(argv[4]);

    srand(time(NULL));

    // Allocate space for the points in memory
    struct coordinate *data_points = (struct coordinate*)malloc((order + 1) * sizeof(struct coordinate));
    // Take equally spaced points to perform the lagrangian on
    for (int i = 0; i <= order; i++) {
        data_points[i].x = (x_start * (order - i) + x_end * i) / order;
        data_points[i].y = f(data_points[i].x);
    }
    
    // Sample random points through Lagrangian
    sample_points_lagrange(order, data_points);
    // Calculate f(x0) from Newton's Interpolation
    printf("%f\n", newton_interpolation(order, data_points, x_0));

    free(data_points);
}

float f(float x) {
    return 1.0 / (1.0 + 25.0 * x * x);
}

// Implementation of Newton's Interpolation
float newton_interpolation(int order, struct coordinate *data, float x_0) {
    int n = order + 1; // Number of points
    float fdd[n][n]; // n x n matrix of Finite Divided Differences

    for (int i = 0; i < n; i++) {
        fdd[i][0] = data[i].y; // The FDD's of order 0 are just set equal to f(x)
    }

    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            // Set FDD's of order j for n points
            fdd[i][j] = (fdd[i + 1][j - 1] - fdd[i][j - 1]) / (data[i + j].x - data[i].x);
        }
    }

    float xterm = 1, f_x_0 = fdd[0][0];
    for (int i = 1; i < n; i++) {
        // i'th term = f(x_i) * (x - x_0) * (x - x_1) ... (x - x_(i-1))
        xterm *= x_0 - data[i - 1].x;
        f_x_0 += fdd[0][i] * xterm;
    }

    return f_x_0;
}

// Implementation of Lagrange's Interpolation
float lagrange_interpolation(int order, struct coordinate *data, float x_0) {
    float f_x_0 = 0;
    int n = order + 1; // Number of Points

    for (int i = 0; i < n; i++) {
        // Calculate coefficient of f(x_i)
        float product = data[i].y;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                product *= (x_0 - data[j].x) / (data[i].x - data[j].x);
            }
        }

        f_x_0 += product;
    }

    return f_x_0;
}

// Sample random points between x_start and x_end
void sample_points_lagrange(int order, struct coordinate *data) {
    struct coordinate new_points[order + 1];
    
    new_points[0].x = data[0].x;
    new_points[0].y = data[0].y;
    new_points[order].x = data[order].x;
    new_points[order].y = data[order].y;

    for (int i = 1; i < order; i++) {
        // Generate a random point from x_start to x_end and find the function value from the Lagrange Interpolation
        new_points[i].x = (float)rand() / RAND_MAX * (data[order].x - data->x) + data->x;
        new_points[i].y = lagrange_interpolation(order, data, new_points[i].x);
    }
    
    // Copy the values over
    for (int i = 0; i <= order; i++) {
        data[i].x = new_points[i].x;
        data[i].y = new_points[i].y;
    }
}
