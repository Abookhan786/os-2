/**
 * ============================================================================
 * Problem: Bounded Buffer Producer-Consumer using Shared Memory & Semaphores
 * ============================================================================
 * Description:
 *   Parent process acts as the PRODUCER; Child process acts as the CONSUMER.
 *   Communication happens through a circular bounded buffer placed inside
 *   System V Shared Memory (shmget / shmat).
 *   Synchronization is coordinated using 3 POSIX Named Semaphores:
 *     1. mutex : Binary semaphore (initial = 1) for mutual exclusion.
 *     2. empty : Counting semaphore (initial = BUFFER_SIZE) tracking empty slots.
 *     3. full  : Counting semaphore (initial = 0) tracking filled slots.
 *
 * Compilation:
 *   gcc -Wall -o ipc_producer_consumer_shm ipc_producer_consumer_shm.c -lpthread
 *
 * Execution:
 *   ./ipc_producer_consumer_shm
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

#define SEM_MUTEX "/sem_pc_mutex"
#define SEM_EMPTY "/sem_pc_empty"
#define SEM_FULL  "/sem_pc_full"

typedef struct {
    int items[BUFFER_SIZE];
    int in;
    int out;
} shared_data_t;

int main() {
    // 1. Create System V Shared Memory segment
    int shmid = shmget(IPC_PRIVATE, sizeof(shared_data_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory
    shared_data_t *shared_data = (shared_data_t *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Initialize buffer indices
    shared_data->in = 0;
    shared_data->out = 0;

    // 2. Unlink any leftover semaphores with the same names
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);

    // 3. Create POSIX named semaphores
    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT | O_EXCL, 0666, BUFFER_SIZE);
    sem_t *sem_full  = sem_open(SEM_FULL,  O_CREAT | O_EXCL, 0666, 0);

    if (sem_mutex == SEM_FAILED || sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    printf("================================================================\n");
    printf("   IPC: Producer-Consumer (Shared Memory + POSIX Semaphores)    \n");
    printf("   Buffer Capacity: %d items | Total items to produce: %d\n", BUFFER_SIZE, TOTAL_ITEMS);
    printf("================================================================\n");

    // 4. Fork process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CONSUMER (Child Process)
        // ==========================================
        for (int i = 1; i <= TOTAL_ITEMS; i++) {
            // Wait until there is at least one item
            sem_wait(sem_full);
            // Acquire critical section
            sem_wait(sem_mutex);

            int item = shared_data->items[shared_data->out];
            printf("  [Consumer Child] Consumed item %2d from slot %d\n", item, shared_data->out);
            shared_data->out = (shared_data->out + 1) % BUFFER_SIZE;

            // Release critical section
            sem_post(sem_mutex);
            // Signal that an empty slot is available
            sem_post(sem_empty);

            usleep(250000); // 250ms delay to simulate processing
        }

        // Detach shared memory
        shmdt(shared_data);
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PRODUCER (Parent Process)
        // ==========================================
        for (int i = 1; i <= TOTAL_ITEMS; i++) {
            int item = i * 10; // Generated item data

            // Wait until an empty slot is available
            sem_wait(sem_empty);
            // Acquire critical section
            sem_wait(sem_mutex);

            shared_data->items[shared_data->in] = item;
            printf("[Producer Parent] Produced item %2d into slot %d\n", item, shared_data->in);
            shared_data->in = (shared_data->in + 1) % BUFFER_SIZE;

            // Release critical section
            sem_post(sem_mutex);
            // Signal that an item has been produced
            sem_post(sem_full);

            usleep(150000); // 150ms delay
        }

        // Wait for child to finish consuming
        wait(NULL);

        printf("----------------------------------------------------------------\n");
        printf("All %d items successfully produced and consumed!\n", TOTAL_ITEMS);

        // Cleanup resources
        shmdt(shared_data);
        shmctl(shmid, IPC_RMID, NULL);

        sem_close(sem_mutex);
        sem_close(sem_empty);
        sem_close(sem_full);

        sem_unlink(SEM_MUTEX);
        sem_unlink(SEM_EMPTY);
        sem_unlink(SEM_FULL);

        printf("Shared memory and semaphores cleaned up successfully.\n");
    }

    return 0;
}
