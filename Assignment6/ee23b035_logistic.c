/*
 *
 * Roll Number: EE23B035
 * Date: 08/09/2023
 * Version: 1.0.0
 * Description: Generate data for a bifurcation plot from n_iter, r_step, and error.
 * Input:
 * - Argument 1 - n_iter: Number of iterations to truncate.
 * - Argument 2 - r_step: Increment for r.
 * - Argument 3 - error: Error to decide whether a unique stable value of x is obtained.
 * Output: data.txt, which contains data for the bifurcation plot.
 * Indentation: 4 spaces
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int num_it = atoi(argv[1]), length;
    double r_step = atof(argv[2]), tol = atof(argv[3]), x, *stable_x;

    FILE* file_ptr = fopen("data.txt", "w");

    for (float r = 0; r <= 4; r += r_step) {
        // Obtain stable value(s) for x.
        x = 0.5;
        for (int i = 0; i < num_it; i++) {
            x = r * x * (1 - x);
        }
        
        length = 1;

        // Read the values that x alternates between
        stable_x = (double*)malloc(sizeof(double));
        *stable_x = x;

        while (1) {
            x = r * x * (1 - x);
            // Only if the newly obtained value for x has an error larger than tol, it is taken
            // as a distinct value.
            // Otherwise, it is treated as a inaccurate duplicate and the loop is terminated.
            if (fabs(x - *(stable_x)) < tol || length > num_it) {
                break;
            }
            
            // Append the newly obtained stable value of x to the array.
            length++;
            stable_x = realloc(stable_x, (length) * sizeof(double));
            *(stable_x + length - 1) = x;
        }

        // Write the data to the file.
        for (int i = 0; i < length; i++) {
            fprintf(file_ptr, "%f %f\n", r, *(stable_x + i));
        }

        free(stable_x);
    }

    fclose(file_ptr);
}
