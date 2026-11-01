/**
 * ============================================================================
 * Problem: Two-Way (Bidirectional) IPC using Half-Duplex UNIX Pipes
 * ============================================================================
 * Description:
 *   Parent process sends a text string to the child process via Pipe 1.
 *   Child process reads the string, converts all lowercase characters to
 *   uppercase (and vice-versa), and reverses the string.
 *   Child sends the processed string back to the parent via Pipe 2.
 *   Parent reads and displays the final transformed string.
 *
 * Compilation:
 *   gcc -Wall -o ipc_pipe_echo_transform ipc_pipe_echo_transform.c
 *
 * Execution:
 *   ./ipc_pipe_echo_transform
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

// Helper function to reverse the case of every character in a string
void transform_string(char *str) {
    int len = strlen(str);

    // Reverse letter case (e.g. "Hi There" -> "hI tHERE")
    for (int i = 0; i < len; i++) {
        if (isupper((unsigned char)str[i])) {
            str[i] = tolower((unsigned char)str[i]);
        } else if (islower((unsigned char)str[i])) {
            str[i] = toupper((unsigned char)str[i]);
        }
    }
}

int main() {
    int pipe_parent_to_child[2]; // Pipe 1: Parent -> Child
    int pipe_child_to_parent[2]; // Pipe 2: Child -> Parent

    // 1. Create both unidirectional pipes
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("Failed to create pipes");
        exit(EXIT_FAILURE);
    }

    printf("====================================================\n");
    printf("   IPC: Two-Way String Transform using UNIX Pipes   \n");
    printf("====================================================\n");

    // 2. Fork into parent and child processes
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS
        // ==========================================
        // Close unused pipe ends
        close(pipe_parent_to_child[1]); // Not writing to Pipe 1
        close(pipe_child_to_parent[0]); // Not reading from Pipe 2

        char child_buffer[BUFFER_SIZE];
        memset(child_buffer, 0, sizeof(child_buffer));

        // Read message from parent
        ssize_t bytes_read = read(pipe_parent_to_child[0], child_buffer, sizeof(child_buffer) - 1);
        if (bytes_read > 0) {
            child_buffer[bytes_read] = '\0';
            printf("[Child PID %d] Received from Parent : \"%s\"\n", getpid(), child_buffer);

            // Transform string (case toggle + reverse)
            transform_string(child_buffer);
            printf("[Child PID %d] Transformed String   : \"%s\"\n", getpid(), child_buffer);

            // Send back to parent
            write(pipe_child_to_parent[1], child_buffer, strlen(child_buffer) + 1);
        }

        // Close remaining pipe ends and exit
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);
        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS
        // ==========================================
        // Close unused pipe ends
        close(pipe_parent_to_child[0]); // Not reading from Pipe 1
        close(pipe_child_to_parent[1]); // Not writing to Pipe 2

        char message[BUFFER_SIZE];
        printf("[Parent PID %d] Enter a message to send to child: ", getpid());
        if (fgets(message, sizeof(message), stdin) != NULL) {
            // Remove trailing newline if present
            message[strcspn(message, "\r\n")] = '\0';

            printf("[Parent PID %d] Sending to Child    : \"%s\"\n", getpid(), message);
            write(pipe_parent_to_child[1], message, strlen(message) + 1);

            // Read the transformed result back from child
            char response_buffer[BUFFER_SIZE];
            memset(response_buffer, 0, sizeof(response_buffer));

            read(pipe_child_to_parent[0], response_buffer, sizeof(response_buffer) - 1);
            printf("[Parent PID %d] Received from Child : \"%s\"\n", getpid(), response_buffer);
        }

        // Close pipe ends
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        // Wait for child to prevent zombie process
        wait(NULL);
        printf("[Parent PID %d] Child process finished. Exiting.\n", getpid());
    }

    return 0;
}
