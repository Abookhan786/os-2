/**
 * ============================================================================
 * Ex 3 - Question 6: POSIX Shared Memory Producer with Named Pipe Notifications
 * ============================================================================
 * Description:
 *   - Shares a 4 KB shared memory segment with the consumer.
 *   - Initially fills the segment with 512 copies of the 8-byte string "freeeee"
 *     to indicate empty slots (512 slots * 8 bytes = 4096 bytes = 4 KB).
 *   - Produces 1000 strings (reusing freed slots).
 *   - Uses two Named Pipes (FIFOs):
 *       1. /tmp/os_fifo_notify_consumer: Informs consumer of newly written slot.
 *       2. /tmp/os_fifo_notify_producer: Informs producer of slots freed by consumer.
 *   - When all 512 slots become full, the producer blocks and waits for the
 *     consumer to free a slot, demonstrating flow control under memory pressure.
 *
 * Compilation:
 *   gcc -Wall -o shm_posix_producer shm_posix_producer.c -lrt
 *
 * Execution:
 *   Run producer in Terminal 1, then run consumer in Terminal 2:
 *   ./shm_posix_producer
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
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
    // Optional delay between production in microseconds (default: 5000 us = 5 ms)
    useconds_t produce_delay_us = 5000;
    if (argc > 1) {
        produce_delay_us = (useconds_t)atoi(argv[1]);
    }

    printf("===============================================================\n");
    printf("   POSIX Shared Memory Producer (4 KB / 512 Slots of 8-bytes) \n");
    printf("   Target: Exchange %d strings using Named Pipe notifications\n", TOTAL_STRINGS);
    printf("===============================================================\n");

    // 1. Create Named Pipes (FIFOs)
    mkfifo(FIFO_NOTIFY_CONSUMER, 0666);
    mkfifo(FIFO_NOTIFY_PRODUCER, 0666);

    // 2. Create Shared Memory Object with read/write permissions
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("[Producer] shm_open failed");
        exit(EXIT_FAILURE);
    }

    // 3. Configure shared memory segment to exactly 4 KB
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("[Producer] ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // 4. Map shared memory into process address space
    char *ptr = (char *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Producer] mmap failed");
        exit(EXIT_FAILURE);
    }

    // 5. Initialize all 512 slots with 8-byte string "freeeee"
    printf("[Producer] Initializing 4 KB memory with %d copies of \"%s\"...\n", NUM_SLOTS, FREE_STR);
    for (int i = 0; i < NUM_SLOTS; i++) {
        memcpy(ptr + (i * SLOT_SIZE), FREE_STR, SLOT_SIZE);
    }
    printf("[Producer] Shared memory initialization complete.\n");

    // 6. Connect to Named Pipes
    printf("[Producer] Waiting for consumer to connect...\n");
    int fd_notify_consumer = open(FIFO_NOTIFY_CONSUMER, O_WRONLY);
    if (fd_notify_consumer == -1) {
        perror("[Producer] Failed to open consumer notification FIFO");
        exit(EXIT_FAILURE);
    }

    int fd_notify_producer = open(FIFO_NOTIFY_PRODUCER, O_RDONLY);
    if (fd_notify_producer == -1) {
        perror("[Producer] Failed to open producer notification FIFO");
        exit(EXIT_FAILURE);
    }
    printf("[Producer] Consumer connected! Starting transmission...\n\n");

    // 7. Initialize free slot pool (slots 0 to 511 are initially free)
    int free_slots[NUM_SLOTS];
    int free_count = NUM_SLOTS;
    for (int i = 0; i < NUM_SLOTS; i++) {
        free_slots[i] = i;
    }

    // Configure fd_notify_producer to non-blocking to drain freed slots during production
    int flags = fcntl(fd_notify_producer, F_GETFL, 0);
    fcntl(fd_notify_producer, F_SETFL, flags | O_NONBLOCK);

    for (int item = 1; item <= TOTAL_STRINGS; item++) {
        // Drain any newly freed slots notified by the consumer
        int freed_slot;
        while (read(fd_notify_producer, &freed_slot, sizeof(int)) > 0) {
            free_slots[free_count++] = freed_slot;
        }

        // If no slots are available, BLOCK and WAIT for consumer to free a slot
        if (free_count == 0) {
            printf("\n>>> [Producer] SHARED MEMORY FULL (512/512 slots used)! Waiting for consumer to free a slot...\n");
            // Temporarily block until a freed slot notification arrives
            fcntl(fd_notify_producer, F_SETFL, flags); // restore blocking
            if (read(fd_notify_producer, &freed_slot, sizeof(int)) > 0) {
                free_slots[free_count++] = freed_slot;
                printf(">>> [Producer] Slot %d freed by consumer! Resuming production.\n\n", freed_slot);
            }
            fcntl(fd_notify_producer, F_SETFL, flags | O_NONBLOCK); // back to non-blocking
        }

        // Allocate slot from free pool
        int current_slot = free_slots[--free_count];

        // Format 8-byte string (e.g., "OS%05d" -> 7 chars + null terminator = 8 bytes)
        char data_str[SLOT_SIZE];
        snprintf(data_str, SLOT_SIZE, "OS%05d", item);

        // Write string into allocated slot in shared memory
        memcpy(ptr + (current_slot * SLOT_SIZE), data_str, SLOT_SIZE);

        // Notify consumer via named pipe about the written slot index
        write(fd_notify_consumer, &current_slot, sizeof(int));

        printf("[Producer #%4d/%d] Wrote \"%s\" at Slot [%3d] | Available slots: %d\n",
               item, TOTAL_STRINGS, data_str, current_slot, free_count);

        if (produce_delay_us > 0) {
            usleep(produce_delay_us);
        }
    }

    printf("\n[Producer] Finished producing all %d strings. Waiting for final shutdown.\n", TOTAL_STRINGS);

    // Clean up
    close(fd_notify_consumer);
    close(fd_notify_producer);
    munmap(ptr, SHM_SIZE);
    close(shm_fd);

    return 0;
}

