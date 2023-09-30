/*
 * Roll Number: EE23B035
 * Date: 15/09/2023
 * Version: 1.0.0
 * Description: Generate N, M random bits, given a seed, write them to a file, and find the minimum Hamming distance.
 * Input:
 *  - Argument N <int>: Number of bits in nbits
 *  - Argument Nseed <int>: Seed for nbits
 *  - Argument M <int>: Number of bits in mbits
 *  - Argument Mseed <int>: Seed for mbits
 * Output: Bit sequences are written to Nfile.dat and Mfile.dat. The location and value of the minimum Hamming distance are printined.
 * Indentation: 4 spaces
 * Sources: Stack Overflow, to find out if a string contains only digits.
 */

#define OK 0;
#define FATAL_ERROR 2;

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// Struct to store the minimum Hamming distance and it's location.
struct Hamming {
    int location;
    int distance;
};

int check_int(char*);
void write_to_file(int, int*, char*);
void generate_random_bits(int, int*, int);
int hamming_distance(int, int*, int*);
struct Hamming minimum_hamming_distance(int, int*, int, int*);

int main(int argc, char** argv) {
    int N, M, Nseed, Mseed;

    // Make sure all arguments are integers.
    if (!check_int(argv[1])) {
        printf("N is not an integer!\n");
        return FATAL_ERROR;
    }
    if (!check_int(argv[2])) {
        printf("Nseed is not an integer!\n");
        return FATAL_ERROR;
    }
    if (!check_int(argv[3])) {
        printf("M is not an integer!\n");
        return FATAL_ERROR;
    }
    if (!check_int(argv[4])) {
        printf("Mseed is not an integer!\n");
        return FATAL_ERROR;
    }

    N = atoi(argv[1]);
    Nseed = atoi(argv[2]);
    M = atoi(argv[3]);
    Mseed = atoi(argv[4]);
    
    // Ensure N > M.
    if (N <= M) {
        printf("M should be less than N\n!");
        return FATAL_ERROR;
    }

    // Allocate memory for nbits and mbits.
    int *nbits = (int*)malloc(N * sizeof(int)), *mbits = (int*)malloc(M * sizeof(int));

    generate_random_bits(N, nbits, Nseed);
    generate_random_bits(M, mbits, Mseed);
   
    write_to_file(N, nbits, "Nfile.dat");
    write_to_file(M, mbits, "Mfile.dat");
    
    // Evaluate the minimum Hamming distance and print it.
    struct Hamming min_hamming = minimum_hamming_distance(N, nbits, M, mbits);
    printf("%d %d\n", min_hamming.location, min_hamming.distance);
    
    free(nbits);
    free(mbits);

    return OK;
}

// Check if a given string can be typecasted into an integer.
int check_int(char* string) {
    int is_num = 1;
    while (*string) {
        is_num = is_num && isdigit(*string);
        *string++; // Move to next character.
    }

    return is_num;
}

// Generate random bits, given the length of the array, the pointer to the array, and a seed.
void generate_random_bits(int num_bits, int* bits_ptr, int seed) {
    srand(seed);

    for (int i = 0; i < num_bits; i++) {
        *(bits_ptr + i) = rand() & 1;
    }
}

// Write the bits array to a given file.
void write_to_file(int num_bits, int* bits_ptr, char* file) {
    FILE* file_ptr = fopen(file, "w");
    
    for (int i = 0; i < num_bits; i++) {
        fprintf(file_ptr, "%d ", *(bits_ptr + i));
    }

    fclose(file_ptr);
}

// Calculate the Hamming distance given two arrays of the same length `num_bits`.
int hamming_distance(int num_bits, int* arr1, int* arr2) {
    int distance = 0;

    for (int i = 0; i < num_bits; i++) {
        distance += *(arr1 + i) ^ *(arr2 + i);
    }

    return distance;
}

// Calculate the minimum Hamming distance over two arrays of different lengths.
struct Hamming minimum_hamming_distance(int N, int* nbits, int M, int* mbits) {
    struct Hamming hamming;
    hamming.distance = M;
    hamming.location = 1;

    int temp_hamming = 0;

    for (int nbits_start = 0; nbits_start <= N - M; nbits_start++) {
        temp_hamming = hamming_distance(M, nbits + nbits_start, mbits);
        // If the evaluated hamming distance for the current subarray is smaller, set that to the minimum Hamming distance.
        if (temp_hamming < hamming.distance) {
            hamming.distance = temp_hamming;
            hamming.location = nbits_start + 1;
        }
    }

    return hamming;
}
