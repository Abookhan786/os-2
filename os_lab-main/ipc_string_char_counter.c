/**
 * ============================================================================
 * Problem: String Character & Vowel Analysis using Two-Way UNIX Pipes
 * ============================================================================
 * Description:
 *   Parent process takes an input string from the user (or command-line argument)
 *   and transmits it to the Child process via Pipe 1.
 *
 *   Child process reads the string and computes comprehensive character stats:
 *     - Vowels count (A, E, I, O, U / a, e, i, o, u)
 *     - Consonants count
 *     - Digits count (0-9)
 *     - Whitespace / Spaces count
 *     - Special characters count
 *     - Word count
 *     - Total character length
 *
 *   Child packages the counts into a structured data packet (string_stats_t)
 *   and transmits it back to the Parent process via Pipe 2.
 *   Parent process receives the struct and displays an aligned summary table.
 *
 * Compilation:
 *   gcc -Wall -o ipc_string_char_counter ipc_string_char_counter.c
 *
 * Execution:
 *   ./ipc_string_char_counter
 *   or:
 *   ./ipc_string_char_counter "Operating Systems Lab 2026! #1"
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFFER_SIZE 512

// Structure containing all analyzed metrics
typedef struct {
    int vowels;
    int consonants;
    int digits;
    int spaces;
    int special_chars;
    int words;
    int total_chars;
} string_stats_t;

// Check if character is an English vowel
bool is_vowel(char c) {
    c = tolower((unsigned char)c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

// Analyze the string and populate stats
string_stats_t analyze_string(const char *str) {
    string_stats_t stats = {0, 0, 0, 0, 0, 0, 0};
    bool in_word = false;

    stats.total_chars = strlen(str);

    for (int i = 0; i < stats.total_chars; i++) {
        char ch = str[i];

        // 1. Check for Letters (Vowels vs Consonants)
        if (isalpha((unsigned char)ch)) {
            if (is_vowel(ch)) {
                stats.vowels++;
            } else {
                stats.consonants++;
            }
        }
        // 2. Check for Numeric Digits
        else if (isdigit((unsigned char)ch)) {
            stats.digits++;
        }
        // 3. Check for Whitespace
        else if (isspace((unsigned char)ch)) {
            stats.spaces++;
        }
        // 4. Special / Punctuation Characters
        else {
            stats.special_chars++;
        }

        // 5. Word count tracking
        if (!isspace((unsigned char)ch) && !in_word) {
            in_word = true;
            stats.words++;
        } else if (isspace((unsigned char)ch)) {
            in_word = false;
        }
    }

    return stats;
}

int main(int argc, char *argv[]) {
    int pipe_parent_to_child[2]; // Pipe 1: Parent -> Child (sends raw string)
    int pipe_child_to_parent[2]; // Pipe 2: Child -> Parent (sends stats struct)

    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("Failed to create pipes");
        exit(EXIT_FAILURE);
    }

    printf("=================================================================\n");
    printf("   IPC: String Character, Vowel & Word Counter using Pipes       \n");
    printf("=================================================================\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Analyzer)
        // ==========================================
        // Close unused pipe ends
        close(pipe_parent_to_child[1]); // Not writing to Pipe 1
        close(pipe_child_to_parent[0]); // Not reading from Pipe 2

        char text[BUFFER_SIZE];
        memset(text, 0, sizeof(text));

        // Read string from parent
        ssize_t bytes_read = read(pipe_parent_to_child[0], text, sizeof(text) - 1);
        if (bytes_read > 0) {
            text[bytes_read] = '\0';
            printf("\n[Child PID %d] Received string from Parent: \"%s\"\n", getpid(), text);
            printf("[Child PID %d] Analyzing character distribution...\n", getpid());

            // Compute statistics
            string_stats_t stats = analyze_string(text);

            // Send computed stats struct back to Parent
            write(pipe_child_to_parent[1], &stats, sizeof(string_stats_t));
            printf("[Child PID %d] Analysis complete. Sent stats to Parent.\n", getpid());
        }

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

        char input_string[BUFFER_SIZE];
        memset(input_string, 0, sizeof(input_string));

        // Get string from command line or stdin
        if (argc >= 2) {
            strncpy(input_string, argv[1], sizeof(input_string) - 1);
        } else {
            printf("[Parent PID %d] Enter a string to analyze: ", getpid());
            if (fgets(input_string, sizeof(input_string), stdin) != NULL) {
                // Strip trailing newline
                input_string[strcspn(input_string, "\r\n")] = '\0';
            }
        }

        // Send string to child
        printf("[Parent PID %d] Sending string to Child: \"%s\"\n", getpid(), input_string);
        write(pipe_parent_to_child[1], input_string, strlen(input_string) + 1);

        // Read analysis results struct from Child
        string_stats_t result;
        ssize_t bytes_read = read(pipe_child_to_parent[0], &result, sizeof(string_stats_t));

        if (bytes_read == sizeof(string_stats_t)) {
            printf("\n=================================================================\n");
            printf("                  CHARACTER ANALYSIS REPORT                      \n");
            printf("=================================================================\n");
            printf("  Original Input     : \"%s\"\n", input_string);
            printf("-----------------------------------------------------------------\n");
            printf("  Vowels (A,E,I,O,U) : %3d\n", result.vowels);
            printf("  Consonants         : %3d\n", result.consonants);
            printf("  Digits (0-9)       : %3d\n", result.digits);
            printf("  Whitespaces        : %3d\n", result.spaces);
            printf("  Special Characters : %3d\n", result.special_chars);
            printf("-----------------------------------------------------------------\n");
            printf("  Total Words        : %3d\n", result.words);
            printf("  Total Characters   : %3d\n", result.total_chars);
            printf("=================================================================\n");
        } else {
            fprintf(stderr, "[Parent] Error receiving stats packet from Child.\n");
        }

        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        wait(NULL); // Wait for child to exit
        printf("[Parent PID %d] Done.\n", getpid());
    }

    return 0;
}
