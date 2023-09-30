/*
 * Roll Number: EE23B035
 * Date: 18/09/2023
 * Version: 1.0.0
 * Description: Given `N` planets and `M` requests, report whether a bridge needs to be constructed for each request, along with the total time of execution using Quick Weighted Union Find (structs and pointers).
 * Input:
 *  - Argument: File to read input from
 * Output: The status of each request (1 or 0) and the execution time is written to ee23b035_quiz2_q3_output.txt.
 * Indentation: 4 spaces
 * Sources: https://courses.cs.duke.edu/cps100e/fall09/notes/UnionFind.pdf
 */

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* 
 * Planets are connected in a tree format.
 * The weight of a planet is indicative of the number of planets below it in the tree.
 * If the parent field points to itself, it means that it is the root node of the tree.
 */
struct Planet {
    int weight; // Number of planets connected to this
    struct Planet* parent; // Pointer to it's parent
};

void fill_planets(struct Planet**, int); // Initialise planets
struct Planet* get_root(struct Planet*); // Given a planet, find the root of the tree it is present in
int connect_bridge(int, int, struct Planet**, int); // Connect two planets if not already connected

int main(int argc, char** argv) {
    // Start the timer
    clock_t begin = clock();

    int n, num_requests, x, y;
    struct Planet **planets; // Array to store pointer to each of the planets
    FILE *input_file_ptr = fopen(argv[1], "r");
    FILE *output_file_ptr = fopen("ee23b035_quiz2_q3_output.txt", "w");

    // Read the number of planets and requests from the gods
    fscanf(input_file_ptr, "%d %d", &n, &num_requests);
    planets = (struct Planet**)malloc(n * sizeof(struct Planet*)); // Allocate memory for the planets
    fill_planets(planets, n);

    while(num_requests--) {
        fscanf(input_file_ptr, "%d %d", &x, &y);
        fprintf(output_file_ptr, "%d", connect_bridge(x, y, planets, n));
    }

    fprintf(output_file_ptr, "\n");
    
    // Stop the timer and calculate the time taken
    clock_t end = clock();
    double time_spent = 1000 * (double)(end - begin) / CLOCKS_PER_SEC;
    fprintf(output_file_ptr, "%d ms\n", (int)ceil(time_spent));

    fclose(input_file_ptr);
    fclose(output_file_ptr);

    // Free memory that has been allocated
    for (int i = 0; i < n; i++)
        free(planets[i]);
    free(planets);
}

// Number each planet from 0 to n - 1 and assign them to a tree
void fill_planets(struct Planet **planets, int n) {
    for (int i = 0; i < n; i++) {
        struct Planet *p = (struct Planet*)malloc(sizeof(struct Planet));
        p->parent = p; // Initially, each planet is the root of the tree
        p->weight = 1; // There is only 1 planet in each tree initially
        *(planets + i) = p;
    }
}

// Given a tree, find the root
struct Planet* get_root(struct Planet* planet) {
    while (planet->parent != planet) {
        planet = planet->parent->parent;
    }
    return planet;
}

// Given two planets, check whether a bridge is required to connect them
int connect_bridge(int index1, int index2, struct Planet** planets, int n) {
    struct Planet *planet1 = planets[index1], *planet2 = planets[index2];
    struct Planet *parent1 = get_root(planet1), *parent2 = get_root(planet2);
    
    // The two planets have the same parent, so they are already connected
    if (parent1 == parent2) {
        return 0;
    }
    
    // Parent1 should have more planets connected to it.
    // This is so that the tree branches as much as possible and will thus
    // need less iterations to find the parent node.
    if (parent2->weight > parent1->weight) {
        struct Planet* temp = parent1;
        parent1 = parent2;
        parent2 = temp;
    }
    
    // Connect parent2 to parent1.
    parent1->weight += parent2->weight;
    parent2->parent = parent1;

    return 1;
}
