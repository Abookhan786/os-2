/*
 * ============================================================================
 * MODEL QUESTION 3: SLIDING WINDOW PROTOCOL SIMULATION (C LANGUAGE)
 * Application: Media Production House (Raw Video Transfer to Render Farm)
 * Description: Simulates pipelined transmission of multiple video frames
 *              within a sliding window (W > 1) before receiving ACKs.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#define MAX_FRAMES 50

/* Visual representation of the sliding window buffer */
void display_window(int base, int next_seq, int window_size, int total_frames) {
    printf("  Buffer Status: ");
    for (int i = 0; i < total_frames; i++) {
        if (i == base) printf("[ ");
        if (i >= base && i < next_seq) {
            printf("F%d* ", i);   // Transmitted and in flight
        } else if (i >= next_seq && i < base + window_size) {
            printf("F%d  ", i);   // Within window, eligible to send
        } else {
            printf("F%d  ", i);   // Outside window
        }
        if (i == base + window_size - 1 || (base + window_size > total_frames && i == total_frames - 1)) {
            printf("] ");
        }
    }
    printf(" (* = in-flight unACKed)\n");
}

int main() {
    int total_frames = 10;
    int window_size = 4;
    char choice[16];

    printf("================================================================================\n");
    printf("       MEDIA PRODUCTION HOUSE: SLIDING WINDOW PROTOCOL SIMULATION               \n");
    printf("   Transferring Raw 4K Video Frames to Remote Render Farm (High Efficiency)     \n");
    printf("================================================================================\n\n");

    printf("Default Configuration:\n");
    printf("  Total Video Frames: 10 frames (Raw 4K ProRes 4444 chunks)\n");
    printf("  Sliding Window Size: 4 frames (Pipelined transmission)\n");
    printf("Press Enter to run default or type 'custom' to enter values: ");

    if (fgets(choice, sizeof(choice), stdin) != NULL && strncmp(choice, "custom", 6) == 0) {
        printf("Enter total number of video frames (e.g. 8-20): ");
        if (scanf("%d", &total_frames) != 1) total_frames = 10;
        printf("Enter window size W (e.g. 2-6): ");
        if (scanf("%d", &window_size) != 1) window_size = 4;
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("               STARTING PIPELINED SLIDING WINDOW TRANSMISSION                   \n");
    printf("--------------------------------------------------------------------------------\n");

    int base = 0;          // Oldest unacknowledged frame
    int next_seq_num = 0;  // Next frame sequence number to transmit
    int round = 1;

    while (base < total_frames) {
        printf("\n>>> [ROUND %02d] Window: [Frames %d to %d]\n",
               round++, base, (base + window_size - 1 < total_frames) ? base + window_size - 1 : total_frames - 1);

        display_window(base, next_seq_num, window_size, total_frames);

        // 1. Transmit all eligible frames within current window
        while (next_seq_num < base + window_size && next_seq_num < total_frames) {
            printf("  [STUDIO TX] Sending Video Frame F%02d (Chunk: Scene_01_Frame_%02d.raw)...\n",
                   next_seq_num, next_seq_num);
            SLEEP_MS(300);
            next_seq_num++;
        }

        printf("  -- Pipelined batch sent. Waiting for Render Farm ACK --\n");
        SLEEP_MS(600);

        // 2. Render Farm receives and sends ACK for the base frame
        // In sliding window, receiver acknowledges the frame and window slides
        int ack_num = base;
        printf("  [RENDER FARM RX] Frame F%02d processed successfully.\n", ack_num);
        printf("  [RENDER FARM TX] >>> Sending Cumulative ACK for Frame F%02d >>>\n", ack_num);

        // 3. Studio receives ACK and slides the window forward
        base++;
        printf("  [STUDIO RX] Received ACK for F%02d -> Sliding Window Advances to base Frame F%02d!\n",
               ack_num, (base < total_frames) ? base : total_frames);

        SLEEP_MS(400);
    }

    printf("\n================================================================================\n");
    printf("                 TRANSMISSION COMPLETE & EFFICIENCY ANALYSIS                    \n");
    printf("================================================================================\n");
    printf("All %d Raw Video Frames successfully received and verified at Render Farm.\n\n", total_frames);

    // Efficiency comparison
    double a = 2.0; // Ratio of propagation delay to transmission delay (Tp / Tt)
    double stop_wait_eff = (1.0 / (1.0 + 2.0 * a)) * 100.0;
    double sliding_window_eff = ((double)window_size / (1.0 + 2.0 * a));
    if (sliding_window_eff > 1.0) sliding_window_eff = 1.0;
    sliding_window_eff *= 100.0;

    printf("Performance Comparison (for Link delay parameter a = Tp/Tt = %.1f):\n", a);
    printf("  1. Stop-and-Wait Protocol (W = 1)   : Efficiency = 1 / (1 + 2a) = %.2f%%\n", stop_wait_eff);
    printf("  2. Sliding Window Protocol (W = %d)  : Efficiency = W / (1 + 2a) = %.2f%%\n", window_size, sliding_window_eff);
    printf("  >> Throughput Improvement Factor   : %.1fx FASTER than Stop-and-Wait!\n", sliding_window_eff / stop_wait_eff);
    printf("================================================================================\n");

    return 0;
}
