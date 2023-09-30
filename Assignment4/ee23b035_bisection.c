/*
 * Roll Number: EE23B035
 * Date: 1/09/2023
 * Version: 1.0.0
 * Description: Estimate the root of f(x) = -25 + 82x - 90 x^2 + 44 x^3 - 8 x^4 + 0.7 x^5 through bisection method and false position method.
 * Input:
 * - Argument: 1 for bisection and 2 for false position.
 * Output: The estimated value for the root of f(x) is printed.
 * Indentation: 4 spaces
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int product_is_positive(float, float); // Check if product of two numbers is positive.
float approx_bisection(float, float, float, float, float); // Approximate the root through bisection method.
float approx_false_position(float, float, float, float, float, float); // Approximate the root through false position method.

// Function to obtain root from
float f(float x) {
    return -25 + 82 * x - 90 * powf(x, 2) + 44 * powf(x, 3) - 8 * powf(x, 4) + 0.7 * powf(x, 5);
}


int main(int argc, char** argv) {
    float x_lower = 0.5, x_upper = 1.0, bisection_tolerance = 0.1, false_position_tolerance = 0.002; // Initialise boundary values and tolerances
    float f_x_lower = f(x_lower), f_x_upper = f(x_upper); // Calculate f(x) at the boundaries
    
    int choice = atoi(argv[1]);
    
    if (choice == 1)
        printf("%f", approx_bisection(x_lower, x_upper, f_x_lower, f_x_upper, bisection_tolerance));
    else
        printf("%f", approx_false_position(x_lower, x_upper, f_x_lower, f_x_upper, f_x_lower, false_position_tolerance));
}


float approx_bisection(float x_lower, float x_upper, float f_x_lower, float f_x_upper, float tolerance) {
    float x_guess = (x_lower + x_upper) / 2; // Compute the midpoint of the range, for the next guess.
    float err = fabsf((x_lower - x_upper) / (x_lower + x_upper)); // Calculate the error associated with the guess.

    if (err < tolerance) {
        return x_guess;
    }

    float f_x_guess = f(x_guess);
    
    if (f_x_guess == 0) {
        return x_guess; // Guess is correct.
    } else if (product_is_positive(f_x_upper, f_x_guess)) {
        return approx_bisection(x_lower, x_guess, f_x_lower, f_x_guess, tolerance); // Take the lower range.
    } else {
        return approx_bisection(x_guess, x_upper, f_x_guess, f_x_upper, tolerance); // Take the upper range.
    }
}


float approx_false_position(float x_lower, float x_upper, float f_x_lower, float f_x_upper, float f_prev_x_guess, float tolerance) {
    float x_guess = x_upper - (f_x_upper * (x_upper - x_lower)) / (f_x_upper - f_x_lower); // Calculate the next guess.
    float f_x_guess = f(x_guess); // Find f(guess)
    float err = fabsf((f_x_guess - f_prev_x_guess) / f_x_guess); // Calculate the error associated with the guess.

    if (err < tolerance) {
        return x_guess;
    }
    
    if (f_x_guess == 0) {
        return x_guess; // Guess is correct.
    } else if (product_is_positive(f_x_upper, f_x_guess)) {
        return approx_false_position(x_lower, x_guess, f_x_lower, f_x_guess, f_x_upper, tolerance); // Take the lower range.
    } else {
        return approx_false_position(x_guess, x_upper, f_x_guess, f_x_upper, f_x_lower, tolerance); // Take the upper range.
    }
}


int product_is_positive(float a, float b) {
    int a_pos = a > 0, b_pos = b > 0;
    return (a_pos && b_pos) || (!a_pos && !b_pos);
}
