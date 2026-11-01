/**
 * ============================================================================
 * Ex 3 - Question 6: POSIX Shared Memory Consumer with Named Pipe Notifications
 * ============================================================================
 * Description:
 *   - Attaches to the 4 KB POSIX shared memory segment created by the producer.
 *   - Consumer permissions are modified to PROT_READ | PROT_WRITE and O_RDWR
 *     to allow writing back to shared memory.
 *   - Receives newly produced slot indices from the producer via Named Pipe:
 *       /tmp/os_fifo_notify_consumer
 *   - Reads and displays the 8-byte string from the notified slot.
 *   - Resets the slot back to "freeeee" to free it up.
 *   - Informs the producer about the freed slot via Named Pipe:
 *       /tmp/os_fifo_notify_producer
 *   - Waits for 1 second after consuming each string (sleep(1)).
 *   - Consumes all 1000 strings and removes the shared memory & FIFOs on completion.
 *
 * Compilation:
 *   gcc -Wall -o shm_posix_consumer shm_posix_consumer.c -lrt
 *
 * Execution:
 *   Run producer in Terminal 1, then run consumer in Terminal 2:
 *   ./shm_posix_consumer [optional_sleep_seconds]
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SHM_NAME "/shm_posix_lab3_q6"
#define FIFO_NOTIFY_CONSUMER "/tmp/os_fifo_notify_consumer"
#define FIFO_NOTIFY_PRODUCER "/tmp/os_fifo_notify_producer"

#define SHM_SIZE 4096
#define SLOT_SIZE 8
#define NUM_SLOTS (SHM_SIZE / SLOT_SIZE) // 512 slots
#define TOTAL_STRINGS 1000
#define FREE_STR "freeeee"

int main(int argc, char *argv[]) {
    // Default consumption delay is 1 second as specified in the lab assignment
    int sleep_seconds = 1;
    if (argc > 1) {
        sleep_seconds = atoi(argv[1]);
    }

    printf("===============================================================\n");
    printf("   POSIX Shared Memory Consumer (4 KB / 512 Slots of 8-bytes) \n");
    printf("   Target: Consume %d strings | Sleep delay: %d second(s)\n", TOTAL_STRINGS, sleep_seconds);
    printf("===============================================================\n");

    // 1. Open the existing Shared Memory segment with Read & Write permissions
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("[Consumer] shm_open failed (Ensure producer is running first)");
        exit(EXIT_FAILURE);
    }

    // 2. Map shared memory with PROT_READ | PROT_WRITE permissions
    char *ptr = (char *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Consumer] mmap failed");
        exit(EXIT_FAILURE);
    }

    // 3. Open Named Pipes (FIFOs)
    // Note: Opening order matches Producer to prevent any blocking deadlocks
    printf("[Consumer] Connecting to notification Named Pipes...\n");
    int fd_notify_consumer = open(FIFO_NOTIFY_CONSUMER, O_RDONLY);
    if (fd_notify_consumer == -1) {
        perror("[Consumer] Failed to open consumer notification FIFO");
        exit(EXIT_FAILURE);
    }

    int fd_notify_producer = open(FIFO_NOTIFY_PRODUCER, O_WRONLY);
    if (fd_notify_producer == -1) {
        perror("[Consumer] Failed to open producer notification FIFO");
        exit(EXIT_FAILURE);
    }
    printf("[Consumer] Connected! Beginning consumption loop...\n\n");

    // 4. Consume 1000 strings
    for (int count = 1; count <= TOTAL_STRINGS; count++) {
        int slot_index;

        // Block until producer sends the index of a newly written slot
        ssize_t bytes_read = read(fd_notify_consumer, &slot_index, sizeof(int));
        if (bytes_read <= 0) {
            printf("[Consumer] Pipe closed or error encountered. Terminating.\n");
            break;
        }

        // Read 8-byte string from the slot in shared memory
        char consumed_data[SLOT_SIZE + 1];
        memcpy(consumed_data, ptr + (slot_index * SLOT_SIZE), SLOT_SIZE);
        consumed_data[SLOT_SIZE] = '\0'; // ensure null termination for printing

        // Display consumed string
        printf("[Consumer #%4d/%d] Read \"%s\" from Slot [%3d]\n",
               count, TOTAL_STRINGS, consumed_data, slot_index);

        // Reset the slot back to "freeeee" in shared memory
        memcpy(ptr + (slot_index * SLOT_SIZE), FREE_STR, SLOT_SIZE);

        // Notify producer that this slot has been freed
        write(fd_notify_producer, &slot_index, sizeof(int));

        // Sleep after consuming each string (lab spec: 1 second)
        if (sleep_seconds > 0) {
            sleep(sleep_seconds);
        }
    }

    printf("\n[Consumer] Successfully consumed all %d strings!\n", TOTAL_STRINGS);
    printf("[Consumer] Cleaning up shared memory and Named Pipes...\n");

    // 5. Cleanup resources
    close(fd_notify_consumer);
    close(fd_notify_producer);
    munmap(ptr, SHM_SIZE);
    close(shm_fd);

    // Unlink shared memory object and FIFOs
    shm_unlink(SHM_NAME);
    unlink(FIFO_NOTIFY_CONSUMER);
    unlink(FIFO_NOTIFY_PRODUCER);

    printf("[Consumer] Cleanup complete. Exiting.\n");
    return 0;
}

