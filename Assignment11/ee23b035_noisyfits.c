/*
 * Roll Number: EE23B035
 * Date: 27/10/2023
 * Description: Fit a Gaussian onto a noisy Lorentzian and find the goodness of fit R^2 and estimate the amplitude, and FWHM.
 * Input:
 *  - Argument: Number of points
 *  - Argument: sigma_N: Standard deviation of normal noise.
 * Output: sigma_N A sigma_g R^2
 * Indentation: 4 spaces
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define X_START -2.0
#define X_END 2.0

struct coordinate {
    float x;
    float y;
};

float f(float); // Lorentzian
float box_muller_transform(); // Implementation of Box Muller Transform
void generate_noise(float*, int, float, float); // Generate normal noise with standard deviation sigma_g
void find_R_square(char*, float*, float*, float*); // Find R^2, sigma_g and A using GNUPlot
void generate_peak(struct coordinate*, int, float); // Generate a Lorentzian peak
void write_data(char*, struct coordinate*, float*, int); // Write the noisy data to a file

int main(int argc, char **argv) {
    int N = atoi(argv[1]);
    float sigma_N = atof(argv[2]);
    float delta_x = (X_END - X_START) / N;

    float *noise = (float*)malloc(N * sizeof(float));
    struct coordinate *data = (struct coordinate*)malloc(N * sizeof(struct coordinate));

    generate_noise(noise, N, delta_x, sigma_N);
    generate_peak(data, N, delta_x);
    write_data("data.txt", data, noise, N);

    free(data);
    free(noise);
    
    float A, B, R_2;
    find_R_square("data.txt", &A, &B, &R_2);

    printf("%f %f %f %f\n", sigma_N, A, B, R_2);
}

// Lorentzian with FWHM = 10
float f(float x) {
    return 1.0 / (1.0 + 25.0 * x * x);
}

float box_muller_transform() {
    float U1, U2;
    
    // Generate random numbers from a uniform distribution over 0 to 1
    U1 = (float)rand() / RAND_MAX;
    U2 = (float)rand() / RAND_MAX;
    return fmodf(sqrtf(-2.0 * logf(U1)) * cosf(2 * M_PI  * U2), 1.0);
}

// Generate normal noise with standard deviation sigma_g for N points
void generate_noise(float *data, int N, float delta_x, float sigma_N) {
    for (int i = 0; i < N; i++) {
        data[i] = box_muller_transform() * sigma_N;
    }
}

// Generate a Lorentzian peak with N points evenly spaced delta_x distance
void generate_peak(struct coordinate *data, int N, float delta_x) {
    for (int i = 0; i < N; i++) {
        data[i].x = X_START + delta_x * i;
        data[i].y = f(data[i].x);
    }
}

// Write the data to a given file
void write_data(char *file, struct coordinate *data, float *noise, int N) {
    FILE *f_ptr = fopen(file, "w");
    
    for (int i = 0; i < N; i++) {
        fprintf(f_ptr, "%.5f %.5f\n", data[i].x, data[i].y + noise[i]);
    }

    fclose(f_ptr);
}

// Use GNUPlot to estimate sigma_g, A, and R^2
void find_R_square(char *file, float *A, float *B, float *R_2) {

    char command[1000];
    sprintf(
        command,
        "gnuplot -p -e \"mean(x) = m; fit mean(x) '%s' using 1:2 via m; f(x) = A * exp(- x ** 2 / (2 * B ** 2)); SST = FIT_WSSR; fit f(x) '%s' using 1:2 via A,B; SSE=FIT_WSSR; set print \\\"temp.txt\\\"; print A, B, 1 - SSE / SST;\"  > /dev/null 2>/dev/null",
        file,
        file
    );
    
    system(command);

    FILE *f_ptr = fopen("temp.txt", "r");
    fscanf(f_ptr, "%f %f %f", A, B, R_2);
    fclose(f_ptr);
}
