/**
 * ============================================================================
 * Problem: Parallel Sorting & Statistical Analysis via Shared Memory
 * ============================================================================
 * Description:
 *   Parent process initializes an unsorted integer array in Shared Memory.
 *   Child process sorts the array in ascending order and computes the median.
 *   Parent waits for child to finish, then reads the sorted array directly
 *   from shared memory, displays the result, and cleans up memory segments.
 *
 * Compilation:
 *   gcc -Wall -o ipc_array_sort_shm ipc_array_sort_shm.c
 *
 * Execution:
 *   ./ipc_array_sort_shm
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_SIZE 10

typedef struct {
    int data[MAX_SIZE];
    int size;
    double median;
    int is_sorted; // Flag set by child when done
} shared_array_t;

int main() {
    // 1. Allocate shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(shared_array_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach to shared memory
    shared_array_t *shm = (shared_array_t *)shmat(shmid, NULL, 0);
    if (shm == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Sample initial dataset
    int initial_values[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    shm->size = MAX_SIZE;
    shm->is_sorted = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
        shm->data[i] = initial_values[i];
    }

    printf("===============================================================\n");
    printf("   IPC: Array Sorting & Median Calculation via Shared Memory   \n");
    printf("===============================================================\n");

    printf("[Parent PID %d] Original Array: ", getpid());
    for (int i = 0; i < shm->size; i++) {
        printf("%d ", shm->data[i]);
    }
    printf("\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Sorter)
        // ==========================================
        printf("[Child PID %d] Sorting array in shared memory...\n", getpid());

        // Bubble sort algorithm
        for (int i = 0; i < shm->size - 1; i++) {
            for (int j = 0; j < shm->size - i - 1; j++) {
                if (shm->data[j] > shm->data[j + 1]) {
                    int tmp = shm->data[j];
                    shm->data[j] = shm->data[j + 1];
                    shm->data[j + 1] = tmp;
                }
            }
        }

        // Calculate median
        int mid = shm->size / 2;
        if (shm->size % 2 == 0) {
            shm->median = (shm->data[mid - 1] + shm->data[mid]) / 2.0;
        } else {
            shm->median = shm->data[mid];
        }

        shm->is_sorted = 1;
        shmdt(shm);
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS
        // ==========================================
        wait(NULL); // Wait for child to complete sorting

        printf("[Parent PID %d] Child finished sorting.\n", getpid());
        printf("[Parent PID %d] Sorted Array  : ", getpid());
        for (int i = 0; i < shm->size; i++) {
            printf("%d ", shm->data[i]);
        }
        printf("\n");
        printf("[Parent PID %d] Computed Median: %.2f\n", getpid(), shm->median);

        // Detach and deallocate shared memory
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        printf("[Parent PID %d] Shared memory deallocated successfully.\n", getpid());
    }

    return 0;
}
