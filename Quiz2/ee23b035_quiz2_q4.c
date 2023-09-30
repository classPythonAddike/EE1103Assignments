/*
 * Roll Number: EE23B035
 * Date: 18/09/2023
 * Version: 1.0.0
 * Description: Given `N` planets and `M` requests, report whether a bridge needs to be constructed for each request, along with the total time of execution using Quick Weighted Union Find (roots and weights array).
 * Input:
 *  - Argument: File to read input from
 * Output: The status of each request (1 or 0) and the execution time is written to ee23b035_quiz2_q4_output.txt.
 * Indentation: 4 spaces
 * Sources: https://courses.cs.duke.edu/cps100e/fall09/notes/UnionFind.pdf
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void fill_planets(int*, int*, int); // Initialise planets
int find_parent(int, int*); // Given a planet, find the root of the tree it is present in
int connect_bridge(int, int, int*, int*, int); // Connect two planets if not already connected

int main(int argc, char** argv) {
    clock_t begin = clock();

    int *roots, *weights; // Roots array to store each planet's tree root, and weights array to store number of elements in each tree
    int n, num_requests, x, y;

    FILE *input_file_ptr = fopen(argv[1], "r");
    FILE *output_file_ptr = fopen("ee23b035_quiz2_q4_output.txt", "w");
    
    // Read the number of planets and requests from the gods
    fscanf(input_file_ptr, "%d %d", &n, &num_requests);
    // Allocate memory for the planet roots and weights array
    roots = (int*)malloc(n * sizeof(int));
    weights = (int*)malloc(n * sizeof(int));

    fill_planets(roots, weights, n);

    while(num_requests--) {
        fscanf(input_file_ptr, "%d %d", &x, &y);
        fprintf(output_file_ptr, "%d", connect_bridge(x, y, roots, weights, n));
    }

    fprintf(output_file_ptr, "\n");
    
    // Stop timer and calculate time elapsed
    clock_t end = clock();
    double time_spent = 1000 * (double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(output_file_ptr, "%d ms\n", (int)ceil(time_spent));

    // Close files and free allocated memory
    fclose(input_file_ptr);
    fclose(output_file_ptr);
    free(roots);
    free(weights);
}

// Number each planet from 0 to n - 1 and assign them to their own tree
void fill_planets(int *roots, int* weights, int n) {
    for (int i = 0; i < n; i++) {
        *(roots + i) = i; // Each planet is initially the root node
        *(weights + i) = 1; // Each tree has an initial size of 1
    }
}

// Given a tree, find the root
int find_parent(int planet, int* roots) {
    while (roots[planet] != planet) {
        planet = roots[roots[planet]];
    }
    return planet;
}

// Given two planets, check whether a bridge is required to connect them
int connect_bridge(int index1, int index2, int* roots, int* weights, int n) {
    int parent1 = find_parent(index1, roots), parent2 = find_parent(index2, roots);

    // The two planets have the same parent, so they are already connected
    if (parent1 == parent2) {
        return 0;
    }

    // Parent1 should have more planets connected to it.
    // This is so that the tree branches as much as possible and will thus
    // need less iterations to find the parent node.
    if (weights[parent2] > weights[parent1]) {
        int temp = parent1;
        parent1 = parent2;
        parent2 = temp;
    }

    // Connect parent2 to parent1.
    weights[parent1] += weights[parent2];
    roots[parent2] = parent1;

    return 1;
}
