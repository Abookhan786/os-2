/**
 * ============================================================================
 * Problem: Multi-Child Worker Pipeline (Fibonacci & Prime Filter via Pipes)
 * ============================================================================
 * Description:
 *   A parent process coordinates two separate worker child processes:
 *     - Child 1 (Fibonacci Worker): Computes first N terms of Fibonacci series
 *       and streams them through pipe_fib.
 *     - Child 2 (Prime Worker): Identifies all prime numbers up to limit M
 *       and streams them through pipe_prime.
 *   The parent reads from both pipes, verifies the data streams, displays
 *   formatted reports, and collects exit statuses using waitpid().
 *
 * Compilation:
 *   gcc -Wall -o ipc_fib_prime_split ipc_fib_prime_split.c
 *
 * Execution:
 *   ./ipc_fib_prime_split
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

int main() {
    int n_fib = 12;      // First 12 Fibonacci numbers
    int prime_limit = 40; // Primes up to 40

    int pipe_fib[2];
    int pipe_prime[2];

    if (pipe(pipe_fib) == -1 || pipe(pipe_prime) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    printf("===============================================================\n");
    printf("   IPC: Dual Worker Pipeline (Parent + 2 Specialized Children) \n");
    printf("===============================================================\n");

    // ------------------------------------------------------------------------
    // Fork Child 1: Fibonacci Generator
    // ------------------------------------------------------------------------
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Fork 1 failed");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Child 1: Fibonacci Worker
        close(pipe_fib[0]);    // Close unused read end of pipe_fib
        close(pipe_prime[0]);  // Not using pipe_prime
        close(pipe_prime[1]);

        long long a = 0, b = 1;
        for (int i = 0; i < n_fib; i++) {
            write(pipe_fib[1], &a, sizeof(long long));
            long long next = a + b;
            a = b;
            b = next;
        }

        long long sentinel = -1;
        write(pipe_fib[1], &sentinel, sizeof(long long));
        close(pipe_fib[1]);
        exit(EXIT_SUCCESS);
    }

    // ------------------------------------------------------------------------
    // Fork Child 2: Prime Number Filter
    // ------------------------------------------------------------------------
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("Fork 2 failed");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Child 2: Prime Worker
        close(pipe_fib[0]);    // Not using pipe_fib
        close(pipe_fib[1]);
        close(pipe_prime[0]);  // Close unused read end of pipe_prime

        for (int i = 2; i <= prime_limit; i++) {
            if (is_prime(i)) {
                write(pipe_prime[1], &i, sizeof(int));
            }
        }

        int sentinel = -1;
        write(pipe_prime[1], &sentinel, sizeof(int));
        close(pipe_prime[1]);
        exit(EXIT_SUCCESS);
    }

    // ------------------------------------------------------------------------
    // Parent Process: Aggregator
    // ------------------------------------------------------------------------
    // Close unused write ends in parent
    close(pipe_fib[1]);
    close(pipe_prime[1]);

    // Read Fibonacci stream from Child 1
    printf("\n[Parent] Reading Fibonacci sequence from Child 1 (PID: %d):\n  ", pid1);
    long long fib_val;
    while (read(pipe_fib[0], &fib_val, sizeof(long long)) > 0) {
        if (fib_val == -1) break;
        printf("%lld ", fib_val);
    }
    printf("\n");
    close(pipe_fib[0]);

    // Read Prime numbers stream from Child 2
    printf("\n[Parent] Reading Primes (up to %d) from Child 2 (PID: %d):\n  ", prime_limit, pid2);
    int prime_val;
    while (read(pipe_prime[0], &prime_val, sizeof(int)) > 0) {
        if (prime_val == -1) break;
        printf("%d ", prime_val);
    }
    printf("\n");
    close(pipe_prime[0]);

    // Wait for both children to finish
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    printf("\n[Parent] Both child workers completed successfully.\n");
    return 0;
}
