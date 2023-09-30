/*
 * Roll Number: EE23B035
 * Date: 07/09/2023
 * Version: 1.0.0
 * Description: Given a list of integers, create a linked list, and sort it using the quick sort algorithm.
 * Input:
 * - Argument: File to read integers from.
 * Output: Sorted linked list
 * Indentation: 4 spaces
 *
 */

#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* next;
};

void push(struct Node**, int); // Push a new integer onto the linked list, given its head.
struct Node* tail(struct Node*); // Obtain the tail of a linked list, given its head.
void print_linked_list(struct Node*); // Print the linked list, given its head.
struct Node* quicksort(struct Node*); // Quick sort the linked list, given its head.

int main(int argc, char** argv) {
    struct Node* head = NULL;
    int n, err;
    FILE* file_ptr = fopen(argv[1], "r");

    // Read the data.
    while (1) {
        err = fscanf(file_ptr, "%d", &n);
        if (err == -1) {
            break;
        }

        push(&head, n);
    }

    // Sort and print.
    head = quicksort(head);
    print_linked_list(head);
}

void push(struct Node** head, int data) {
    // Allocate space for the node.
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    // Store the integer and update the head.
    node->data = data;
    node->next = *head;
    *head = node;
}

void print_linked_list(struct Node* head) {
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }

    printf("\n");
}

struct Node* tail(struct Node* head) {
    while (head->next != NULL) {
        head = head->next;
    }

    return head;
}

struct Node* quicksort(struct Node* head) {
    struct Node* pivot = tail(head);
    
    if (head == pivot)
        return head;

    int sent_right = 0, sent_left = 0;
    struct Node *less_than = NULL, *greater_than = NULL, *head_cpy = head;
    
    // Partition the list and move smaller elements to left, larger elements to the right.
    while (head != pivot) {
        if (head->data > pivot->data) {
            push(&greater_than, head->data);
            sent_right++;
        } else {
            push(&less_than, head->data);
            sent_left++;
        }
        head_cpy = head;
        head = head->next;
        
        // Delete the node since we created a new one while pushing.
        free(head_cpy);
    }

    // Combine the lists again:
    // Less_than List +  Pivot + Greater_than List
    if (sent_left > 0 && sent_right > 0) {
        less_than = quicksort(less_than);
        greater_than = quicksort(greater_than);
        tail(less_than)->next = pivot;
        pivot->next = greater_than;
        return less_than;
    } else if (sent_left > 0) {
        less_than = quicksort(less_than);
        tail(less_than)->next = pivot;
        return less_than;
    } else if (sent_right > 0) {
        greater_than = quicksort(greater_than);
        pivot->next = greater_than;
        return pivot;
    } else {
        return pivot;
    }
}
