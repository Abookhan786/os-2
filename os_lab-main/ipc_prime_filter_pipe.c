/**
 * ============================================================================
 * Problem: Prime Number Filter from Array using Two-Way UNIX Pipes
 * ============================================================================
 * Description:
 *   Parent process takes an integer array from the user (or command-line arguments)
 *   and sends it to the Child process via Pipe 1.
 *
 *   Child process:
 *     1. Reads the array size and elements from Pipe 1.
 *     2. Filters through the array and identifies only the PRIME numbers.
 *     3. Transmits the filtered prime numbers and their count back to the
 *        Parent process via Pipe 2.
 *
 *   Parent process:
 *     1. Receives the prime numbers from Pipe 2.
 *     2. Displays the original array, the filtered prime numbers, and the count.
 *     3. Waits for the child process to terminate cleanly.
 *
 * Compilation:
 *   gcc -Wall -o ipc_prime_filter_pipe ipc_prime_filter_pipe.c
 *
 * Execution:
 *   ./ipc_prime_filter_pipe
 *   or with command line numbers:
 *   ./ipc_prime_filter_pipe 10 17 20 23 29 35 37 40 41 44
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAX_SIZE 100

// Helper function to check if a number is prime
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    int pipe_parent_to_child[2]; // Pipe 1: Parent -> Child
    int pipe_child_to_parent[2]; // Pipe 2: Child -> Parent

    // 1. Create two unidirectional pipes for full-duplex communication
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    printf("=================================================================\n");
    printf("     IPC: Array Prime Number Filter using UNIX Pipes             \n");
    printf("=================================================================\n");

    // 2. Fork into parent and child
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Prime Filter Worker)
        // ==========================================
        // Close unused pipe ends
        close(pipe_parent_to_child[1]); // Not writing to Pipe 1
        close(pipe_child_to_parent[0]); // Not reading from Pipe 2

        int n = 0;
        // Read array size from Parent
        if (read(pipe_parent_to_child[0], &n, sizeof(int)) <= 0) {
            fprintf(stderr, "[Child PID %d] Error reading array size.\n", getpid());
            close(pipe_parent_to_child[0]);
            close(pipe_child_to_parent[1]);
            exit(EXIT_FAILURE);
        }

        int input_array[MAX_SIZE];
        // Read array elements from Parent
        read(pipe_parent_to_child[0], input_array, n * sizeof(int));

        printf("\n[Child PID %d] Received array of %d numbers from Parent:\n  [ ", getpid(), n);
        for (int i = 0; i < n; i++) {
            printf("%d ", input_array[i]);
        }
        printf("]\n");

        // Filter prime numbers
        int primes[MAX_SIZE];
        int prime_count = 0;

        for (int i = 0; i < n; i++) {
            if (is_prime(input_array[i])) {
                primes[prime_count++] = input_array[i];
            }
        }

        printf("[Child PID %d] Identified %d prime number(s). Sending back to Parent...\n",
               getpid(), prime_count);

        // Send prime count followed by prime elements back to Parent
        write(pipe_child_to_parent[1], &prime_count, sizeof(int));
        if (prime_count > 0) {
            write(pipe_child_to_parent[1], primes, prime_count * sizeof(int));
        }

        // Close pipe ends and exit
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS (Sender & Presenter)
        // ==========================================
        // Close unused pipe ends
        close(pipe_parent_to_child[0]); // Not reading from Pipe 1
        close(pipe_child_to_parent[1]); // Not writing to Pipe 2

        int n = 0;
        int numbers[MAX_SIZE];

        // Gather input from command-line arguments or interactive prompt
        if (argc > 1) {
            n = argc - 1;
            if (n > MAX_SIZE) n = MAX_SIZE;
            for (int i = 0; i < n; i++) {
                numbers[i] = atoi(argv[i + 1]);
            }
        } else {
            printf("[Parent PID %d] How many numbers do you want to send? (1-%d): ", getpid(), MAX_SIZE);
            if (scanf("%d", &n) != 1 || n <= 0) {
                // Default fallback array if input fails
                n = 8;
                int defaults[] = {12, 17, 24, 29, 31, 35, 41, 50};
                for (int i = 0; i < n; i++) numbers[i] = defaults[i];
                printf("Using default array of %d elements.\n", n);
            } else {
                if (n > MAX_SIZE) n = MAX_SIZE;
                printf("[Parent PID %d] Enter %d integer(s) separated by spaces:\n", getpid(), n);
                for (int i = 0; i < n; i++) {
                    scanf("%d", &numbers[i]);
                }
            }
        }

        printf("[Parent PID %d] Sending array to Child process...\n", getpid());
        // 1. Send size
        write(pipe_parent_to_child[1], &n, sizeof(int));
        // 2. Send array elements
        write(pipe_parent_to_child[1], numbers, n * sizeof(int));

        // Read result from Child process via Pipe 2
        int prime_count = 0;
        int prime_results[MAX_SIZE];

        read(pipe_child_to_parent[0], &prime_count, sizeof(int));
        if (prime_count > 0) {
            read(pipe_child_to_parent[0], prime_results, prime_count * sizeof(int));
        }

        // Display results
        printf("\n=================================================================\n");
        printf("                   FILTER RESULTS (AT PARENT)                    \n");
        printf("=================================================================\n");
        printf("Original Array Sent : [ ");
        for (int i = 0; i < n; i++) {
            printf("%d ", numbers[i]);
        }
        printf("] (Total: %d)\n", n);

        printf("Primes Received     : [ ");
        for (int i = 0; i < prime_count; i++) {
            printf("%d ", prime_results[i]);
        }
        printf("] (Count: %d)\n", prime_count);
        printf("=================================================================\n");

        // Close pipe ends
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        // Wait for child to exit
        wait(NULL);
        printf("[Parent PID %d] Child process terminated cleanly.\n", getpid());
    }

    return 0;
}
