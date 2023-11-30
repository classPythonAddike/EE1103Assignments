/*
 * Roll Number: EE23B035
 * Date: 15/10/2023
 * Description: Given a time period (T), number of peaks (M) and width (a), generate Lorentzians/Gaussians with noise and estimate the time periods and widths from the noisy data in real time.
 * Input:
 *  - Argument: M - Number of peaks (int)
 *  - Argument: T - Time period (int)
 *  - Argument: a - Width (int)
 *  - Argument: Function (Gaussian or Lorentzian)
 * Output: <T> <a> std_dev(T) std_dev(a)
 * Indentation: 4 spaces
 */

#define POINT_DENSITY 10 // Number of points per unit length - Make sure this is greater than 1 / a and less than 50
#define NOISE_FACTOR_A 0.05
#define NOISE_FACTOR_T 0.05
#define NOISE_FACTOR_N 0.1
#define DATA_FILE "data.txt" // File to write noisy graph to
#define ALPHA 0.8 // Modulation factor while smoothing - recommended to keep this between 0.75 and 0.85

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
int find_peaks(struct peak**, int); // Estimate peaks by reading data point by point from a file
void write_data(struct data*, int); // Write the given data to a file
void generate_noise_n(float*, int); // Generate noise for the data - n 
float gaussian(float, float, float); // Gaussian distribution
float lorentzian(float, float, float); // Lorentzian distribution
void read_next_point(FILE*, struct data*); // Read the next data point from the file
float calculate_stdev_T(struct peak*, int, float); // Find standard deviation in T
float calculate_stdev_a(struct peak*, int, float); // Find standard deviation in a
void generate_noise_A(float*, float, float); // Generate noise for a - eps_a
void generate_noise_T(float*, float, float); // Generate noise for T - eps_T
float calculate_average_T(struct peak*, int); // Find average T
float calculate_average_a(struct peak*, int); // Find average a
void smooth_data(struct data*, struct data*); // Smooth the data using a weighted average filter
void generate_noisy_peaks(struct data*, struct peak*, float*, float*, float*, float, float, float, int); // Generate peaks with noise

int function = 0; // 0 for Lorentzian, 1 for Gaussian

int main(int argc, char **argv) {
    const float M = atof(argv[1]), T = atof(argv[2]), A = atof(argv[3]);
    const int num_points = POINT_DENSITY * M * T; // Total number of samples

    if (argc == 5) {
        function = argv[4][0] == 'g' || argv[4][0] == 'G';
    }

    if (A > 0.05 * T) {
        printf("Please make `a` smaller or `T` larger (a < 5%% of T)!\n");
        exit(2);
    } else if ((float)POINT_DENSITY < 1.0 / A) {
        printf("Please make sure the point density is less than 1 / `a`!\n");
        exit(2);
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
    
    // Generate data and write them to the data file
    generate_noisy_peaks(data_points, peaks, epsilon_A, epsilon_T, n, A, T, M, num_points);
    write_data(data_points, num_points);
    
    // Delete all variables related to the generated data
    free(n);
    free(peaks);
    free(epsilon_A);
    free(epsilon_T);
    free(data_points);

    // Find peaks in real time
    struct peak* estimated_peaks;
    int num_estimated_peaks = find_peaks(&estimated_peaks, num_points);
    // Find mean and stdev of T and a    
    float mean_T = calculate_average_T(estimated_peaks, num_estimated_peaks), mean_a = calculate_average_a(estimated_peaks, num_estimated_peaks);
    float stdev_T = calculate_stdev_T(estimated_peaks, num_estimated_peaks, mean_T), stdev_a = calculate_stdev_a(estimated_peaks, num_estimated_peaks, mean_a);
    printf("%f %f %f %f\n", mean_T, mean_a, stdev_T, stdev_a);

    free(estimated_peaks);
}

// Write the given data to a file
void write_data(struct data *datapoints, int num_points) {
    FILE* file_ptr = fopen(DATA_FILE, "w");

    for (int i = 0; i < num_points; i++) {
        fprintf(file_ptr, "%f %f\n", datapoints[i].x, datapoints[i].y);
    }

    fclose(file_ptr);
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

// Estimate the location, time period, half width and amplitude of peaks in real time
int find_peaks(struct peak** estimated_peaks, int num_points) {
    int num_estimated_peaks = 0;
    struct data *previous_point = (struct data*)malloc(sizeof(struct data)), *current_point = (struct data*)malloc(sizeof(struct data));
    FILE* file_ptr = fopen(DATA_FILE, "r");

    int looking_for_peaks = 0; // Boolean telling us whether we are searching for a peak or not
    float max_amplitude = 0, max_amplitude_x = 0; // Amplitude of last peak, position of last peak

    current_point->x = 0;
    current_point->y = 0;

    for (int i = 0; i < num_points; i++) {
        // Move forward by a point
        *previous_point = *current_point;
        read_next_point(file_ptr, current_point);
        
        smooth_data(previous_point, current_point); // Use an exponential filter
        
        // If we are not looking for a peak and we encounter points higher than the half maximum of the previous peak, start searching for a peak
        if (!looking_for_peaks && max_amplitude / 2 < current_point->y) {
            looking_for_peaks = 1;
            max_amplitude /= 2; // Our new largest points seen are the two current points
            max_amplitude_x = current_point->x;
        // If we are look for peaks, and we find points higher than the current maximum, set the maximum to those two points
        } else if (looking_for_peaks && current_point->y > max_amplitude) {
            max_amplitude = previous_point->y;
            max_amplitude_x = current_point->x;
        // If we are looking for peaks and encounter a point which is the half maximum, then we stop looking for peaks and add a new peak to our guesses
        } else if (looking_for_peaks && (current_point->y) - max_amplitude / 2 < 0) {
            looking_for_peaks = 0;
            num_estimated_peaks++;
            *estimated_peaks = (struct peak*)realloc(*estimated_peaks, num_estimated_peaks * sizeof(struct peak)); // Add memory for another peak
            (*estimated_peaks)[num_estimated_peaks - 1].location = max_amplitude_x;
            (*estimated_peaks)[num_estimated_peaks - 1].T = 2 * (*estimated_peaks)[num_estimated_peaks - 1].location / (2 * num_estimated_peaks - 1);
            (*estimated_peaks)[num_estimated_peaks - 1].amplitude = max_amplitude;
            (*estimated_peaks)[num_estimated_peaks - 1].a = previous_point->x - max_amplitude_x;
        }
    }

    fclose(file_ptr);
    free(previous_point);
    free(current_point);

    return num_estimated_peaks;
}

// Read the next data point from the file
void read_next_point(FILE* file_ptr, struct data *point) {
    fscanf(file_ptr, "%f %f", &(point->x), &(point->y));
}

// Smoothen the data using an exponential filter
void smooth_data(struct data* previous_point, struct data* current_point) {
    current_point->y = current_point->y * (1 - ALPHA) + previous_point->y * ALPHA;
}

// Find mean value of T
float calculate_average_T(struct peak* estimated_peaks, int num_estimated_peaks) {
    float sum_T = 0;
    
    for (int i = 0; i < num_estimated_peaks; i++) {
        sum_T += estimated_peaks[i].T;
    }
    return sum_T / num_estimated_peaks;
}

// Find standard deviation in T
float calculate_stdev_T(struct peak* estimated_peaks, int num_estimated_peaks, float mean_T) {
    float mean_T_square = 0;
    
    for (int i = 0; i < num_estimated_peaks; i++) {
        mean_T_square += estimated_peaks[i].T * estimated_peaks[i].T / num_estimated_peaks;
    }
    return sqrtf(fabsf(mean_T_square - mean_T * mean_T));
}

// Find mean value of a
float calculate_average_a(struct peak* estimated_peaks, int num_estimated_peaks) {
    float sum_a = 0;
    
    for (int i = 0; i < num_estimated_peaks; i++) {
        sum_a += estimated_peaks[i].a;
    }
    return sum_a / num_estimated_peaks;
}

// Find standard deviation in a
float calculate_stdev_a(struct peak* estimated_peaks, int num_estimated_peaks, float mean_a) {
    float mean_a_square = 0;
    
    for (int i = 0; i < num_estimated_peaks; i++) {
        mean_a_square += estimated_peaks[i].a * estimated_peaks[i].a / num_estimated_peaks;
    }
    return sqrtf(fabsf(mean_a_square - mean_a * mean_a));
}
