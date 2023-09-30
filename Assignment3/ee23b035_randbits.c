/*
 * Roll Number: EE23B035
 * Date: 25/08/2023
 * Version:
 * Description: Program to generate N random bits and write them to a file. If provided two files, the Hamming distance between the two is calculated.
 * Input:
 *  - Flag n <int>: Number of bits to generate
 *  - Flag s <int>: Seed
 *  - Flag t: Use timestamp as seed
 *  - Flag h <file> <file>: Two files to read bits from and calculate Hamming distance
 * Output: Random bits are written to randbits.txt. If the -h flag is provided, Hamming distance is printed.
 * Indentation: 4 spaces
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int parse_flags(int, char**);
void generate_bits(char*);
int hamming_distance();

int N = 0; // Number of bits to generate
int t_flag, h_flag, s_flag = 0; // Flags for h, t, and s
unsigned int seed; // Seed to initialise srand() with
char *h_file1 = NULL, *h_file2 = NULL; // Files to read from while calculating Hamming distance
char *output_file = "randbits.txt"; // File to write random bits to

int main(int argc, char** argv) {
    // Try parsing the flags, exit if any errors.
    int successful = parse_flags(argc, argv);
    if (!successful) {
        return 1;
    }

    // If a seed is provided, set the seed for rand().
    if (s_flag) {
        srand(seed);
    }

    // Generate and write random bits to the output file.
    generate_bits(output_file);

    // Calculate Hamming distance if the -h flag is set.
    if (h_flag) {
        printf("Hamming distance is %d\n", hamming_distance());
    }
}

// Parse the command line arguments.
int parse_flags(int argc, char** argv) {
    opterr = 0; // Silence getopt errors.
    int c;

    while ((c = getopt(argc, argv, "th:n:s:")) != -1) {
        switch (c) {
            case 'n': // Number of bits to generate
                N = atoi(optarg);
                break;
            
            case 's': // User provided seed
                if (s_flag) {
                    printf("Seed has been set with both -t and -s!\n");
                    return 0;
                }
                seed = atoi(optarg);
                s_flag = 1;
                break;

            case 't': // Use timestamp for seed
                if (s_flag) {
                    printf("Seed has been set with both -t and -s!\n");
                    return 0;
                }
                seed = time(NULL);
                s_flag = 1;
                break;
            
            case 'h': // Calculate Hamming distance
                h_flag = 1;
                h_file1 = argv[optind - 1];
                h_file2 = argv[optind];
                break;
        }
    }

    if (N < 1) {
        printf("Please provide a valid argument for N through the -n flag!\n");
        return 0;
    }

    return 1;
}

// Generate N random bits and write them to the output file.
void generate_bits(char* filename) {
    FILE* file_ptr = fopen(filename, "w");

    for (int i = 0; i < N; i++) {
        fprintf(file_ptr, "%d", rand() & 1);
    }

    fclose(file_ptr);
}

// Calculate Hamming distance for the two given files.
int hamming_distance() {
    FILE* file1_ptr = fopen(h_file1, "r");
    FILE* file2_ptr = fopen(h_file2, "r");

    char bit1, bit2;
    int hamming_distance = 0;

    // Loop over each bit and exit when we reach an EOF.
    while ((bit1 = fgetc(file1_ptr)) != EOF && (bit2 = fgetc(file2_ptr)) != EOF) {
        hamming_distance += bit1 != bit2;
    }

    fclose(file1_ptr);
    fclose(file2_ptr);

    return hamming_distance;
}
