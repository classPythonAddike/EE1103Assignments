/*
 * Roll Number: EE23B035
 * Date: 02/10/2023
 * Description: Given a time period (T), number of peaks (M) and width (a), generate Lorentzians/Gaussians with noise and estimate the time periods and widths from the noisy data.
 * Input:
 *  - Argument: M - Number of peaks (int)
 *  - Argument: T - Time period (int)
 *  - Argument: a - Width (int)
 *  - Argument: Function (Gaussian or Lorentzian)
 * Output: <T> <a> std_dev(T) std_dev(a)
 * Indentation: 4 spaces
 */

#define POINT_DENSITY 10 // Number of points per unit length - Make sure this is greater than 5
#define BRACKET_WIDTH 5 // Width of bracket while searching for peaks
#define NOISE_FACTOR_A 0.05
#define NOISE_FACTOR_T 0.05
#define NOISE_FACTOR_N 0.1

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Struct to represent a coordinate
struct data {
    float x;
    float y;
};

// Struct to represent a peak
struct peak {
    float amplitude;
    float location;
    float a;
    float T;
};

float box_muller_transform(); // Generate random floats from a normal distribution using the Box Muller Transform
void generate_noise_n(float*, int); // Generate noise for the data - n 
float gaussian(float, float, float); // Gaussian distribution
void smooth_data(struct data*, int); // Smooth the data using a weighted average filter
float find_stdev_T(struct peak*, int); // Find the standard deviation in the value of T for each predicted peak
float lorentzian(float, float, float); // Lorentzian distribution
float find_average_a(struct peak*, int); // Find the average a for all peaks
float find_average_T(struct peak*, int); // Find the average T for all peaks
float find_threshold(struct data*, int); // Determine a threshold to decide whether a region is a peak or not
void generate_noise_A(float*, float, float); // Generate noise for a - eps_a
void generate_noise_T(float*, float, float); // Generate noise for T - eps_T
float weighted_average(struct data*, int); // Find the weighted average of points around the given point to smoothen it
float find_stdev_a(struct peak*, float, int); // Find the stadard deviation in the value of a for each predicted peak
void find_peak_widths(struct peak*, struct data*, int, int); // Given an estimated peak, find the FWHM
int find_peaks(struct data*, int, float, int, struct peak**); // Find the peaks in the given data
void generate_noisy_peaks(struct data*, struct peak*, float*, float*, float*, float, float, float, int); // Generate peaks with noise

int function = 0; // 0 for Lorentzian, 1 for Gaussian

int main(int argc, char **argv) {
    const float M = atof(argv[1]), T = atof(argv[2]), A = atof(argv[3]);
    const int num_points = POINT_DENSITY * M * T; // Total number of samples

    if (argc == 5) {
        function = argv[4][0] == 'g' || argv[4][0] == 'G';
    }
    
    struct data *data_points = (struct data*)malloc(sizeof(struct data) * num_points); // Data array
    struct peak *peaks       = (struct peak*)malloc(sizeof(struct peak) * M); // Array of peaks
    float *epsilon_A         = (float*)malloc(M * sizeof(float)); // Noise in A
    float *epsilon_T         = (float*)malloc(M * sizeof(float)); // Noise in T
    float *n                 = (float*)malloc(num_points * sizeof(float)); // Noise in n
    
    srand(time(NULL)); // Seed with current timestamp
    // Generate noise arrays
    generate_noise_n(n, num_points);
    generate_noise_T(epsilon_T, T, M);
    generate_noise_A(epsilon_A, A, M);

    generate_noisy_peaks(data_points, peaks, epsilon_A, epsilon_T, n, A, T, M, num_points);
    smooth_data(data_points, num_points);
    
    // Threshold is Average y + Standard Deviation
    float threshold = find_threshold(data_points, num_points);
    
    // Allocate memory for estimation of peaks
    struct peak *estimated_peaks = (struct peak*)malloc(sizeof(struct peak));
    int num_predicted_peaks = find_peaks(data_points, num_points, threshold, BRACKET_WIDTH, &estimated_peaks);
    find_peak_widths(estimated_peaks, data_points, num_predicted_peaks, num_points);
    
    float average_T = find_average_T(estimated_peaks, num_predicted_peaks);
    float std_dev_T = find_stdev_T(estimated_peaks, num_predicted_peaks);
    float average_a = find_average_a(estimated_peaks, num_predicted_peaks);
    float std_dev_a = find_stdev_a(estimated_peaks, average_a, num_predicted_peaks);

    printf("%f %f %f %f\n", average_T, average_a, std_dev_T, std_dev_a);
    
    free(n);
    free(peaks);
    free(epsilon_A);
    free(epsilon_T);
    free(data_points);
    free(estimated_peaks);
}

// Choose two random variables, apply a Box Muller Transform and choose a value from -1 to 1
float box_muller_transform() {
    float U1, U2;
    
    // Generate random numbers from a uniform distribution over 0 to 1
    U1 = (float)rand() / RAND_MAX;
    U2 = (float)rand() / RAND_MAX;
    return fmodf(sqrtf(-2.0 * logf(U1)) * cosf(2 * M_PI  * U2), 1.0);
}

// Generate eps_a using a Box Muller Transform
void generate_noise_A(float* epsilon_A, float A, float M) {
    for (int m = 0; m < M; m++) {
        epsilon_A[m] = A * box_muller_transform() * NOISE_FACTOR_A;
    }
}

// Generate eps_T using a Box Muller Transform
void generate_noise_T(float* epsilon_T, float T, float M) {
    for (int m = 0; m < M; m++) {
        epsilon_T[m] = T * box_muller_transform() * NOISE_FACTOR_T;
    }
}

// Generate noise n(t) using a Box Muller Transform
void generate_noise_n(float* n, int num_points) {
    for (int i = 0; i < num_points; i++) {
        n[i] = box_muller_transform() * NOISE_FACTOR_N;
    }
}

// Find the Lorentzian given the peak location and a
float lorentzian(float x, float peak_location, float a) {
   return (a * a)  / (powf((x - peak_location), 2) + a * a);
}

// Find the Gaussian given the peak location and a
float gaussian(float x, float peak_location, float a) {
    return powf(M_E, -1 * powf((x - peak_location), 2) / (2 * a * a));
}

// Generate noisy peaks (either Lorentzian or Gaussian) given noise in a, T and n(t)
void generate_noisy_peaks(struct data *data_points, struct peak *peaks, float* epsilon_A, float* epsilon_T, float* n, float A, float T, float M, int num_points) {
    for (int m = 0; m < M; m++) {
        peaks[m].a = (A + epsilon_A[m]); // Noisy a for the weak
        peaks[m].location = epsilon_T[m] + ((float)m + 0.5) * T; // Location of the peak
        peaks[m].amplitude = 1; // The peak of a Lorentzian or Gaussian is always 1

        for (int i = 0; i < POINT_DENSITY * T; i++) {
            int data_index            = i + m * T * POINT_DENSITY; // Index of the current point in the data array
            float x                   = (float)(m * T) + (float)i / POINT_DENSITY; // x coordinate of the current point
            data_points[data_index].x = x;

            if (function == 0) {
                data_points[data_index].y = (1 + n[data_index]) * lorentzian(x, peaks[m].location, peaks[m].a);
            } else {
                data_points[data_index].y = (1 + n[data_index]) * gaussian(x, peaks[m].location, peaks[m].a);
            }
        }
    }
}

// Find the weighted average of the previous and next `width` readings.
float weighted_average(struct data *start, int width) {
    float sum = 0;
    float sum_weights = 0;
    for (int i = -width; i <= width; i++) {
        if (i != 0) {
            sum += (start + i + width)->y / fabsf((float)i);
            sum_weights += 1.0 / fabsf((float)i);
        } else {
            sum += 2 * (start + width)->y;
            sum_weights += 2;
        }
    }

    return sum / sum_weights;
}

// Smoothen the data using a weighted moving average.
void smooth_data(struct data *data_points, int num_points) {
    int width = 3;
    for (int i = width; i < num_points - width; i++) {
        data_points[i].y = weighted_average(data_points + i - width, width);
    }
}

// Find a threshold to determine if a region qualifies as a peak or not
// We define the threshold as mean of y + standard deviation of y
float find_threshold(struct data *data_points, int num_points) {
    float mean_y = 0, mean_y_square = 0;
    for (int i = 0; i < num_points; i++) {
        mean_y += data_points[i].y / num_points;
        mean_y_square += powf(data_points[i].y, 2) / num_points;
    }

    return mean_y + sqrtf(mean_y_square - mean_y * mean_y);
}

// Estimate the location of the peaks, given a threshold, precision and the array of data
// The precision is the minimum x we bracket the start and end of peaks
// Return the number of peaks found
int find_peaks(struct data *data_points, int num_points, float threshold, int bracket_width, struct peak** estimated_peaks) {
    
    int num_peaks = 0;
    int x_start = 0, x_end = 0; // Index of the start and end points of the bracket
    int x_peak_end = 0, x_peak_start = 0; // Index of the start and end points of the peak
    for (int i = 0; i < num_points / bracket_width; i++) {
        x_start = bracket_width * i, x_end = bracket_width * (i + 1);
        if (data_points[x_start].y <= threshold && data_points[x_end].y >= threshold) {
            // Found a peak
            x_peak_start = (x_start + x_end) / 2;
            // Search for the end of the peak
            while (data_points[x_end].y >= threshold) {
                i++;
                x_end += bracket_width;
            }

            x_start = x_end - bracket_width;
            x_peak_end = (x_start + x_end) / 2;
            num_peaks += 1;
            
            // Reallocate memory for the new peak
            *estimated_peaks = (struct peak*)realloc(*estimated_peaks, num_peaks * sizeof(struct peak));
            (*estimated_peaks)[num_peaks - 1].location = data_points[(x_peak_end + x_peak_start) / 2].x;
            (*estimated_peaks)[num_peaks - 1].amplitude = data_points[(x_peak_start + x_peak_end) / 2].y; // Amplitude is the y coordinate at the location of the peak
        }
    }

    return num_peaks;
}

// Find the average T for all peaks
float find_average_T(struct peak* estimated_peaks, int num_predicted_peaks) {
    float sum_T = 0;
    int sum_n = 0;
    for (int i = 0; i < num_predicted_peaks; i++) {
        sum_T += estimated_peaks[i].location;
        sum_n += 2 * i + 1;
        estimated_peaks[i].T = (estimated_peaks[i].location * 2) / (2 * i + 1);
    }

    return 2 * sum_T / sum_n;
}

// Find standard deviation in T for all peaks
float find_stdev_T(struct peak *estimated_peaks, int num_predicted_peaks) {
    float exp_T_square = 0, exp_T = 0;
    for (int i = 0; i < num_predicted_peaks; i++) {
        exp_T_square += powf(estimated_peaks[i].T, 2) / num_predicted_peaks;
        exp_T += estimated_peaks[i].T / num_predicted_peaks;
    }

    return sqrtf(exp_T_square - exp_T * exp_T);
}

// Estimate the width of peaks by moving down the peak and measuring the
// width at the point of half-maximum
void find_peak_widths(struct peak *estimated_peaks, struct data *data_points, int num_predicted_peaks, int num_points) {
    // For Lorentzian, the width is 2 * a, and for Gaussian, the width is 2 sqrt(2ln(2)) * a
    float proportionality_constant = function == 0 ? 2 : 2.35482;

    for (int i = 0; i < num_predicted_peaks; i++) {
        float peak_x = estimated_peaks[i].location;
        float peak_y = estimated_peaks[i].amplitude;
        float FWHM = 0;
        int delta_x_forward = 0, delta_x_backward = 0; // Number of datapoints between the half-maximum and maximum
        
        // Move forward and backward until you reach the half-maximum
        for (; data_points[(int)(peak_x * POINT_DENSITY) - delta_x_backward].y > peak_y / 2; delta_x_backward++) {}
        for (; data_points[(int)(peak_x * POINT_DENSITY) + delta_x_forward].y > peak_y / 2; delta_x_forward++) {}
        
        // a = Width / (Proportionality Constant * Point Density)
        estimated_peaks[i].a = (float)(delta_x_forward + delta_x_backward) / (proportionality_constant * POINT_DENSITY);
    }
}

// Given the widths of each peak, find the average a
float find_average_a(struct peak *estimated_peaks, int num_predicted_peaks) {
    float sum_a = 0;
    for (int i = 0; i < num_predicted_peaks; i++) {
        sum_a += estimated_peaks[i].a;
    }
    return sum_a / num_predicted_peaks;
}

// Find the standard deviation in a
float find_stdev_a(struct peak *estimated_peaks, float mean_a, int num_predicted_peaks) {
    float exp_a_square = 0;
    for (int i = 0; i < num_predicted_peaks; i++) {
        exp_a_square += powf(estimated_peaks[i].a, 2) / num_predicted_peaks;
    }

    return sqrtf(exp_a_square - mean_a * mean_a);
}
