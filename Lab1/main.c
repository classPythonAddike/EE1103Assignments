/*
 * Project: EE1103Assignments
 * License: MIT
 * Author: Keshav Saravanan - EE23B035
 * Indentation: 4 spaces
 * 
 * 1. Read a data file and extract the latency values.
 * 2. Find the average and standard deviation of the values.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define OK          0; // Return code for OK
#define FATAL_ERROR 2; // Return code for Fatal error

// Find the number of lines in the data file, given a file pointer.
int num_data_lines(FILE *file_ptr) {
    int n = 0;

    for (char c = getc(file_ptr); c != EOF; c = getc(file_ptr)) {
        if (c == '\n') {
            n++;
        }
    }

    rewind(file_ptr);
    return n - 5;
}

// Parse the time readings from the data file, given a file pointer, the number of lines, and the data array to write to.
void parse_times(FILE *file_ptr, int lines, float* times) {
    char line[1000];
    fgets(line, 1000, file_ptr); // Read the first line from the file.

    char _[20], s[20];
    for (int i = 0; i < lines; i++) {
        fscanf(file_ptr, "%s %s %s %s %s %s %s %s", _, _, _, _, _, _, s, _);
        char *val;
        val = strtok(s, "=");
        val = strtok(NULL, "=");
        times[i] = atof(val);
    }
}

// Sum up an array of floats, given the array and the length of the array.
float sum(float* data, int length) {
    float sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

// Sum up the squares of an array of floats, given the array and the length of the array.
float sum_square(float* data, int length) {
    float sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i] * data[i];
    }
    return sum;
}


int main(int argc, char** argv) {
    FILE* file_ptr = fopen(argv[1], "r");

    if (file_ptr == NULL) {
        printf("Please provide a file as argument!\n");
        return FATAL_ERROR;
    }

    // Find the number of lines in the file and extract the time readings.
    int total = num_data_lines(file_ptr);
    float values[total];
    parse_times(file_ptr, total, values);

    // Calculate mean and stddev from the formulas
    float exp_x = sum(values, total) / total;
    float exp_x_square = sum_square(values, total) / total;
    float std_dev = sqrtf(exp_x_square - exp_x * exp_x);
    
    printf("Mean = %f and Standard Deviation = %f\n", exp_x, std_dev);
    fclose(file_ptr);

    return OK;
}
