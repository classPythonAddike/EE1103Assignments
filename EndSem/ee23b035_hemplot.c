/*
 * Roll Number: EE23B035
 * Date: 14/11/2023
 * Description: Plot a Hemachandra spiral centered about (x0, y0) and with initial radius r0.
 * Input:
 *  - Argument: x0 - x-coordinate of center
 *  - Argument: y0 - y-coordinate of center
 *  - Argument: r0 - Initial radius
 * Output: Data points for the spiral are written to ee23b035_hemplot.dat.
 * Indentation: 4 spaces
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_IT 20
#define DELTA_ANGLE 0.1

long long hem(long long, long long, long long);

int main(int argc, char **argv) {
    float x0 = atof(argv[1]), y0 = atof(argv[2]), r0 = atof(argv[3]);

    float r = r0, theta = 0;
    
    FILE *f_ptr = fopen("ee23b035_hemplot.dat", "w");
    fprintf(f_ptr, "\"Hemachandra Spiral - x0 = %.2f, y0 = %.2f, r0 = %.2f\"\n", x0, y0, r0);

    for (int i = 2; i <= NUM_IT + 1; i++) {
        long long a = hem(i, 0, 1);
        long long b = hem(i + 1, 0, 1);
        
        float delta_theta = ((float)b / a - 1) * 2 * M_PI;
        
        // Fill out the points in between the i, and i+1 terms.
        for (float angle = theta; angle < theta + delta_theta; angle += DELTA_ANGLE) {
            r =r0 * (b * (angle - theta) + a * (theta + delta_theta - angle)) / delta_theta;
            float x = x0 + r * cosf(angle);
            float y = y0 + r * sinf(angle);
            fprintf(f_ptr, "%f %f\n", x, y);
        }
        
        theta += delta_theta;
    }

    fclose(f_ptr);
}

// Recursively find the N'th term in the Hemachandras series
long long hem(long long N, long long a, long long b) {
    if (N == 1) {
        return a;
    }
    return hem(N - 1, b, a + b);
}
