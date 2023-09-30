/*
 * Roll Number: EE23B035
 * Date: 18/09/2023
 * Version: 1.0.0
 * Description: Given `N` planets and `M` requests, report whether a bridge needs to be constructed for each request, along with the total time of execution.
 * Input:
 *  - Argument: File to read input from
 * Output: The status of each request (1 or 0) and the execution time is written to ee23b035_quiz2_q1_output.txt.
 * Indentation: 4 spaces
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void fill_planets(int*, int); // Initialise the planets array
int connect_bridge(int, int, int*, int); // Connect two planets by a bridge

int main(int argc, char** argv) {
    // Start the timer
    clock_t begin = clock();

    int *planets, n, num_requests, x, y;
    FILE *input_file_ptr = fopen(argv[1], "r");
    FILE *output_file_ptr = fopen("ee23b035_quiz2_q1_output.txt", "w");
    
    // Read the number of planets and number of requests from the gods
    fscanf(input_file_ptr, "%d %d", &n, &num_requests);
    planets = (int*)malloc(n * sizeof(int)); // Allocate space for the planets
    fill_planets(planets, n);

    while(num_requests--) {
        fscanf(input_file_ptr, "%d %d", &x, &y);
        fprintf(output_file_ptr, "%d", connect_bridge(x, y, planets, n));
    }

    fprintf(output_file_ptr, "\n");
    
    // Stop the timer
    clock_t end = clock();
    // Calculate the time taken for execution
    double time_spent = 1000 * (double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(output_file_ptr, "%d ms\n", (int)ceil(time_spent));

    // Close files and free up allocated space
    fclose(input_file_ptr);
    fclose(output_file_ptr);
    free(planets);
}

// Number each planet from 1 to n - 1
void fill_planets(int *planets, int n) {
    for (int i = 0; i < n; i++) {
        *(planets + i) = i;
    }
}

// Given two planet indices, connect them if they are not already connected
int connect_bridge(int index1, int index2, int* planets, int n) {
    if (planets[index1] == planets[index2]) {
        return 0; // They belong to the same set and hence, are already connected
    }
    
    int temp = planets[index2];

    for (int i = 0; i < n; i++) {
        // Move all planets from the second planet's set to the first
        if (planets[i] == temp) {
            planets[i] = planets[index1];
        }
    }

    return 1;
}
