/**
 * ============================================================================
 * Problem: Process Synchronization via UNIX Signals (Ping-Pong Rally)
 * ============================================================================
 * Description:
 *   Parent and Child communicate purely using standard user-defined UNIX signals:
 *     - SIGUSR1 ("Ping") sent from Parent to Child.
 *     - SIGUSR2 ("Pong") sent from Child to Parent.
 *   Each process waits for the other's signal using sigsuspend() / pause(),
 *   handles it via custom signal handlers, prints the turn, and signals back.
 *   After a configured number of volleys (ROUNDS), the rally terminates cleanly.
 *
 * Compilation:
 *   gcc -Wall -o ipc_signals_ping_pong ipc_signals_ping_pong.c
 *
 * Execution:
 *   ./ipc_signals_ping_pong
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_ROUNDS 5

static volatile sig_atomic_t signal_received = 0;

void signal_handler(int sig) {
    (void)sig; // Silence unused warning
    signal_received = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Register handlers for both user signals
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("===============================================================\n");
    printf("   IPC: Signal-Based Synchronization (Parent-Child Ping-Pong)   \n");
    printf("   Rounds to play: %d\n", MAX_ROUNDS);
    printf("===============================================================\n");

    pid_t parent_pid = getpid();
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Pong Receiver & Responder)
        // ==========================================
        for (int round = 1; round <= MAX_ROUNDS; round++) {
            // Wait for SIGUSR1 from parent
            while (!signal_received) {
                pause();
            }
            signal_received = 0;

            printf("  [Child PID %d] Received PING -> Sending PONG (Round %d)\n", getpid(), round);
            usleep(200000); // 200ms delay for readability

            // Send SIGUSR2 back to parent
            kill(parent_pid, SIGUSR2);
        }

        printf("  [Child PID %d] Game complete. Exiting.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS (Initiator)
        // ==========================================
        // Small delay to ensure child signal handler is ready
        usleep(100000);

        for (int round = 1; round <= MAX_ROUNDS; round++) {
            printf("[Parent PID %d] Sending PING to Child... (Round %d)\n", getpid(), round);
            kill(pid, SIGUSR1);

            // Wait for SIGUSR2 response from child
            while (!signal_received) {
                pause();
            }
            signal_received = 0;

            printf("[Parent PID %d] Received PONG from Child!\n\n", getpid());
            usleep(200000);
        }

        // Wait for child to exit cleanly
        wait(NULL);
        printf("[Parent PID %d] All rounds finished successfully.\n", getpid());
    }

    return 0;
}
