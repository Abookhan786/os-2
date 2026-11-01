/**
 * ============================================================================
 * Ex 3 - Question 5: String Case Reversal using System V Shared Memory
 * ============================================================================
 * Description:
 *   - Implements Question 4 using Shared Memory (shmget / shmat).
 *   - Variables shared between processes are declared outside main().
 *   - Parent (Process 1) writes an input string (e.g., "Hi There") into
 *     shared memory.
 *   - Child (Process 2) reads the string, toggles the case of each character
 *     ('a'-'z' -> 'A'-'Z', 'A'-'Z' -> 'a'-'z'), and writes the modified string
 *     back into shared memory.
 *   - Parent reads and displays the modified string (e.g., "hI tHERE").
 *   - Clean turn-taking is synchronized using a state variable in shared memory.
 *
 * Compilation:
 *   gcc -Wall -o case_reversal_shm case_reversal_shm.c
 *
 * Execution:
 *   ./case_reversal_shm
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER_SIZE 256

// Shared structure between processes declared outside main() as per lab rules
typedef struct {
    char message[BUFFER_SIZE];
    int status; // 0 = empty, 1 = parent wrote original, 2 = child modified case
} shm_case_buffer_t;

shm_case_buffer_t *shared_buf;

// Reverses the case of every alphabetical character in str
void reverse_case(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (isupper((unsigned char)str[i])) {
            str[i] = tolower((unsigned char)str[i]);
        } else if (islower((unsigned char)str[i])) {
            str[i] = toupper((unsigned char)str[i]);
        }
    }
}

int main() {
    // 1. Create System V Shared Memory Segment
    int shmid = shmget(IPC_PRIVATE, sizeof(shm_case_buffer_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // 2. Attach shared memory
    shared_buf = (shm_case_buffer_t *)shmat(shmid, NULL, 0);
    if (shared_buf == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared status
    shared_buf->status = 0;
    memset(shared_buf->message, 0, BUFFER_SIZE);

    printf("========================================================\n");
    printf("   IPC: String Case Reversal using Shared Memory        \n");
    printf("========================================================\n");

    // 3. Fork child process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Process 2)
        // ==========================================
        // Wait until parent has written the string
        while (shared_buf->status != 1) {
            usleep(1000); // 1 ms sleep to yield CPU
        }

        printf("[Child PID %d] Read from shared memory : \"%s\"\n", getpid(), shared_buf->message);

        // Reverse the case of each character
        reverse_case(shared_buf->message);
        printf("[Child PID %d] Reversed case in memory : \"%s\"\n", getpid(), shared_buf->message);

        // Notify parent that processing is complete
        shared_buf->status = 2;

        // Detach and exit
        shmdt(shared_buf);
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS (Process 1)
        // ==========================================
        const char *sample_text = "Hi There";

        // Write the original string into shared memory
        strncpy(shared_buf->message, sample_text, BUFFER_SIZE - 1);
        shared_buf->message[BUFFER_SIZE - 1] = '\0';

        printf("[Parent PID %d] Original message written: \"%s\"\n", getpid(), shared_buf->message);

        // Signal child that message is ready
        shared_buf->status = 1;

        // Wait until child finishes processing
        while (shared_buf->status != 2) {
            usleep(1000);
        }

        // Wait for child process to terminate cleanly
        wait(NULL);

        printf("\n--------------------------------------------------------\n");
        printf("[Parent PID %d] Final modified string : \"%s\"\n", getpid(), shared_buf->message);
        printf("--------------------------------------------------------\n");

        // Detach and remove shared memory segment
        shmdt(shared_buf);
        shmctl(shmid, IPC_RMID, NULL);
        printf("[Parent] Shared memory detached and destroyed.\n");
    }

    return 0;
}

