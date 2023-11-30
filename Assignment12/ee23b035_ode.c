/*
 * Roll Number: EE23B035
 * Date: 02/11/2023
 * Description: Given a differential equation, estimate the solution using Euler's, Heun's and RK5 approximations, and calculate the goodness of fit relative to RK45
 * Input:
 *  - Argument: Theta_Start - Intial angle in radians
 *  - Argument: Theta_Stop  - Final angle in radians
 *  - Argument: Alpha       - Damping constant
 *  - Argument: Delta_t     - Time step
 * Output: Alpha Delta_t R^2_Euler R^2_Heun
 * Indentation: 4 spaces
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define H 2e-5 // Strength of magnetic field. Make sure this is less than 0.1
#define GAMMA 1.76 * 1e4 * 4 * M_PI // Gyromagnetic Constant

// Coordinate in spherical polar form, on a unit sphere
struct coordinate {
    double theta;
    double phi;
    double time;
};

double THETA_START, THETA_STOP, ALPHA, DELTA_T;

void error(char*);
double phi_dot(double);
double theta_dot(double);
void print(double, double, FILE*);
void sanity_check(double, double, double, double);
double heun_approximation(double, double, double f_dash(double));
double rk45_approximation(double, double, double f_dash(double));
double euler_approximation(double, double, double f_dash(double));
double find_R_square(struct coordinate*, int, struct coordinate*, int);
int simulate_differential_equation(struct coordinate**, double, FILE*, double approximation(double, double, double y_dash(double)));

int main(int argc, char **argv) {
    THETA_START = atof(argv[1]);
    THETA_STOP = atof(argv[2]);
    ALPHA = atof(argv[3]);
    DELTA_T = atof(argv[4]);

    sanity_check(THETA_START, THETA_STOP, ALPHA, DELTA_T);

    struct coordinate *euler_approx, *heun_approx, *RK45_approx;
    
    // Model the differential equations and write the solutions to a file
    FILE *f_ptr = fopen("data.dat", "w");
    fprintf(f_ptr, "\"Euler's Method\"\n");
    int N_euler = simulate_differential_equation(&euler_approx, DELTA_T, f_ptr, euler_approximation);
    fprintf(f_ptr, "\n\n\"Heun's Method\"\n");
    int N_heun = simulate_differential_equation(&heun_approx, DELTA_T, f_ptr, heun_approximation);

    // RK45 is the gold standard - we assume delta_t is proportional to H * Gamma * Alpha
    fprintf(f_ptr, "\n\n\"RK45\"\n");
    int N_RK45 = simulate_differential_equation(&RK45_approx, H * GAMMA * ALPHA * 1e-3, f_ptr, rk45_approximation);
    fclose(f_ptr);
    
    // Assuming RK45 is accurate, find R^2 of Euler's and Heun's methods with respect to RK45
    double R_square_euler = find_R_square(euler_approx, N_euler, RK45_approx, N_RK45);
    double R_square_heun = find_R_square(heun_approx, N_heun, RK45_approx, N_RK45);

    printf("%lf %lf %lf %lf\n", ALPHA, DELTA_T, R_square_euler, R_square_heun);
    // printf("%f\n", H * GAMMA * ALPHA * 1e-3 * N_RK45); // Switching time

    free(euler_approx);
    free(heun_approx);
    free(RK45_approx);
}

void error(char* message) {
    printf("%s\n", message);
    exit(1);
}

// Ensure that inputs are of appropriate order to get a good approximation
void sanity_check(double THETA_START, double THETA_STOP, double ALPHA, double DELTA_T) {
    if (THETA_START < 0 || THETA_STOP > M_PI || THETA_STOP <= THETA_START) {
        error("Please enter a valid Theta_start and Theta_stop: 0 < THETA_START < THETA_STOP < PI");
    } else if (ALPHA > 0.2) {
        error("Alpha should be less than 0.2");
    } else if (DELTA_T <= 0 || ALPHA <= 0) {
        error("Alpha and Delta_t should be greater than 0");
    } else if (H * GAMMA > 10) {
        error("Ensure that H is smaller than 10 / Gamma for a good approximation");
    }
}

// Differential equation for theta
double theta_dot(double theta) {
    return ALPHA * H * sinf(theta) * GAMMA / (ALPHA * ALPHA + 1);
}

// Differential equation for phi
double phi_dot(double theta) {
    return theta_dot(theta) / (sinf(theta) * ALPHA);
}

// Euler's method to model a differential equation
double euler_approximation(double y_i, double delta_t, double y_dash(double)) {
    return y_dash(y_i) * delta_t;
}

// Heun's method to model a differential equation
double heun_approximation(double y_i, double delta_t, double y_dash(double)) {
    double y_i_dash = y_dash(y_i);
    return delta_t * (y_i_dash + y_dash(y_i + y_i_dash * delta_t)) / 2;
}

// Runge-Kutta method
double rk45_approximation(double y_i, double delta_t, double y_dash(double)) {
    double c20 = 0.25, c21 = 0.25;
    double c30 = 0.375, c31 = 0.09375, c32 = 0.28125;
    double c40,c41, c42,c43;
    double c51, c52 = -8.0, c53, c54;
    double c60 = 0.5, c61, c62 = 2, c63, c64;
    double c65 = -0.275;
    double a1, a2 = 0, a3, a4, a5 = -0.2;
    double b1, b2 = 0, b3, b4, b5= -0.18, b6;
    double F1, F2, F3, F4, F5, F6, x4;

    c40 = (double) 12/ (double) 13;
    c41 = (double) 1932/(double) 2197;
    c42 = (double) -7200/(double) 2197;
    c43 = (double) 7296/(double) 2197;
    c51 = c53 = (double) 439/ (double) 216;
    c54 = (double) -845/(double) 4104;
    c61 = (double) -8/(double) 27;
    c63 = (double) -3544/(double) 2565;
    c64 = (double) 1859/(double) 4104;
    a1 = (double) 25/(double) 216;
    a3 = (double) 1408/(double) 2565;
    a4 = (double) 2197/(double) 4104;
    b1 = (double) 16/(double) 135;
    b3 = (double) 6656/(double) 12825;
    b4 = (double) 28561/(double) 56430;
    b6 = (double) 2/(double) 55;

    F1 = delta_t * y_dash(y_i);
    F2 = delta_t * y_dash(y_i + c21 * F1);
    F3 = delta_t * y_dash(y_i + c31 * F1 + c32 * F2);
    F4 = delta_t * y_dash(y_i + c41 * F1 + c42 * F2 + c43 * F3);
    F5 = delta_t * y_dash(y_i + c51 * F1 + c52 * F2 + c53 * F3 + c54 * F4 );
    F6 = delta_t * y_dash(y_i + c61 * F1 + c62 * F2 + c63 * F3 + c64 * F4 + c65 * F5);

    return b1 * F1 + b3 * F3 + b4 * F4 + b5 * F5 + b6 * F6;
}

// Convert a given spherical polar coordinate into cartesian and write it to a file
void print(double theta, double phi, FILE* f_ptr) {
    fprintf(f_ptr, "%lf %lf %lf\n", sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));
}

// Approximate the given differential equation using the given derivative function
int simulate_differential_equation(struct coordinate **data, double delta_t, FILE *f_ptr, double approximation(double, double, double y_dash(double))) {
    int num_points = 1;

    double time = 0;
    double theta = THETA_START;
    double phi = 0;

    *data = (struct coordinate*)malloc(sizeof(struct coordinate));
    (*data)[0].theta = THETA_START;
    (*data)[0].phi = 0;
    (*data)[0].time = 0;

    do {
        print(theta, phi, f_ptr);
        
        // Approximate the increase in phi and theta
        phi += approximation(theta, delta_t, phi_dot);
        theta += approximation(theta, delta_t, theta_dot);
        time += DELTA_T;
        
        // And append it to the data array
        num_points++;
        *data = (struct coordinate*)realloc(*data, num_points * sizeof(struct coordinate));
        
        (*data)[num_points - 1].theta = theta;
        (*data)[num_points - 1].phi = phi;
        (*data)[num_points - 1].time = time;
    } while (theta < THETA_STOP);


    return num_points;
}

// Find the goodness of fit of a given curve compared against the RK45 approximation
double find_R_square(struct coordinate *data, int num_points, struct coordinate *true_data, int num_true_points) {
    double mean = 0;

    for (int i = 0; i < num_true_points; i++) {
        mean += true_data[i].theta / num_true_points;
    }
    
    // SSR = Sum of Squares of Residuals
    // SST = Sum of Squares of Total
    double SSR = 0, SST = 0;

    int pointer = 0;

    for (int i = 0; i < num_points; i++) {
        SST += powf(data[i].theta - mean, 2);
        while (true_data[pointer].theta < data[i].theta && pointer < num_true_points) {
            pointer++;
        }
        SSR += powf(data[i].theta - true_data[pointer].theta, 2);
    }

    return 1 - SSR / SST;
}
