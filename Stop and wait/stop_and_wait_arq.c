/*
 * ============================================================================
 * MODEL QUESTION 4: STOP-AND-WAIT ARQ SIMULATION (C LANGUAGE)
 * Scenario: Delayed ACK arriving after timeout expiration, triggering
 *           sender retransmission & duplicate frame handling at receiver.
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

/* Frame structure */
typedef struct {
    int seq_no;          // 0 or 1 (Alternating Bit Protocol)
    char payload[64];    // Data contents
} Frame;

/* Acknowledgment structure */
typedef struct {
    int ack_no;          // Sequence number of frame being acknowledged
} AckPacket;

/* Receiver state structure */
typedef struct {
    int expected_seq;    // Expected sequence number (0 or 1)
    int duplicate_count; // Number of duplicate frames caught
    int total_accepted;  // Number of unique frames accepted
} ReceiverState;

/*
 * Receiver Function:
 * Handles incoming frame with duplicate filtering logic.
 */
int receiver_process_frame(ReceiverState *rx, Frame f, int handle_duplicates) {
    printf("  [RECEIVER]: Received Frame with Seq No: %d, Payload: \"%s\"\n", f.seq_no, f.payload);

    if (handle_duplicates) {
        // CORRECT ARQ LOGIC:
        if (f.seq_no == rx->expected_seq) {
            // New expected frame received
            printf("  [RECEIVER]: -> NEW FRAME ACCEPTED into application layer!\n");
            rx->expected_seq = 1 - rx->expected_seq; // Toggle expected seq (0 -> 1 -> 0)
            rx->total_accepted++;
            printf("  [RECEIVER]: -> Sending ACK %d to Sender...\n", f.seq_no);
            return 1; // Success
        } else {
            // Duplicate frame detected!
            rx->duplicate_count++;
            printf("  [RECEIVER ALERT]: -> DUPLICATE FRAME DETECTED (Expected %d, got %d)!\n",
                   rx->expected_seq, f.seq_no);
            printf("  [RECEIVER ACTION]: -> DISCARDING duplicate payload data (preventing data corruption).\n");
            printf("  [RECEIVER ACTION]: -> RE-SENDING ACK %d to re-synchronize Sender.\n", f.seq_no);
            return 2; // Duplicate handled
        }
    } else {
        // UNMODIFIED / BUGGY LOGIC (Without duplicate check):
        printf("  [BUGGY RECEIVER]: Blindly accepted Frame %d without sequence check!\n", f.seq_no);
        printf("  [BUGGY RECEIVER WARNING]: DUPLICATE DATA WRITTEN TO APPLICATION BUFFER!\n");
        rx->total_accepted++;
        return 1;
    }
}

int main() {
    int timeout_limit_ms = 1500;  // Sender timeout timer = 1.5 seconds
    int ack_delay_ms = 2200;      // Network delay on ACK = 2.2 seconds (> timeout)

    printf("========================================================================================\n");
    printf("             STOP-AND-WAIT ARQ SIMULATION: DELAYED ACK & DUPLICATE HANDLING             \n");
    printf("========================================================================================\n");
    printf("Simulation Parameters:\n");
    printf("  - Sender Retransmission Timeout : %d ms\n", timeout_limit_ms);
    printf("  - Network Delay on ACK Link     : %d ms (ACK arrives AFTER timeout has expired!)\n", ack_delay_ms);
    printf("========================================================================================\n\n");

    // Initialize Receiver State
    ReceiverState rx;
    rx.expected_seq = 0;
    rx.duplicate_count = 0;
    rx.total_accepted = 0;

    Frame frame0;
    frame0.seq_no = 0;
    strcpy(frame0.payload, "CRITICAL_TRANSACTION_#9021");

    printf(">>> STAGE 1: SENDER TRANSMITS INITIAL FRAME\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[T = 0.0s] [SENDER]: Transmitting Frame [Seq: %d, Data: '%s']\n", frame0.seq_no, frame0.payload);
    printf("[T = 0.0s] [SENDER]: Retransmission timer started (%d ms)...\n\n", timeout_limit_ms);

    SLEEP_MS(300);

    // Receiver gets the frame
    printf("[T = 0.3s] >>> Frame arrives at Receiver >>>\n");
    receiver_process_frame(&rx, frame0, 1);

    printf("\n[T = 0.4s] [NETWORK]: Receiver generates ACK 0, but transmission link experiences HIGH JITTER/DELAY!\n");
    printf("[T = 0.4s] [NETWORK]: ACK 0 is delayed in transit for %d ms...\n\n", ack_delay_ms);

    // Sender waiting for ACK
    printf("[T = 0.4s to 1.5s] [SENDER]: Waiting for ACK 0...\n");
    SLEEP_MS(1200);

    printf("\n----------------------------------------------------------------------------------------\n");
    printf(">>> STAGE 2: TIMEOUT EXPIRES AT SENDER (BEFORE DELAYED ACK ARRIVES)\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[T = 1.5s] [SENDER ALERT]: *** TIMEOUT EXPIRED (Timer reached %d ms) ***\n", timeout_limit_ms);
    printf("[T = 1.5s] [SENDER]: Sender assumes Frame 0 was lost!\n");
    printf("[T = 1.5s] [SENDER]: Retransmitting DUPLICATE Frame [Seq: %d] to Receiver...\n\n", frame0.seq_no);

    SLEEP_MS(700);

    // Now delayed ACK finally arrives at sender
    printf("[T = 2.2s] [SENDER]: Late delayed ACK 0 finally arrives at Sender.\n");
    printf("[T = 2.2s] [SENDER]: (Notice: Retransmitted duplicate frame is already in flight!)\n\n");

    SLEEP_MS(400);

    printf("----------------------------------------------------------------------------------------\n");
    printf(">>> STAGE 3: RECEIVER PROCESSES RETRANSMITTED DUPLICATE FRAME\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("[T = 2.6s] >>> Retransmitted duplicate Frame 0 arrives at Receiver >>>\n");

    // Receiver processes the duplicate frame with modified logic
    receiver_process_frame(&rx, frame0, 1);

    SLEEP_MS(500);

    printf("\n========================================================================================\n");
    printf("                                OBSERVATIONS & SUMMARY                                  \n");
    printf("========================================================================================\n");
    printf("1. Cause of Duplicate Frame:\n");
    printf("   - The acknowledgment ACK 0 took %d ms to arrive, which exceeded the sender's timeout\n", ack_delay_ms);
    printf("     of %d ms. This premature timeout caused the sender to retransmit Frame 0.\n", timeout_limit_ms);
    printf("2. Problem if Unhandled:\n");
    printf("   - Without sequence numbers, the receiver would accept Frame 0 twice, corrupting the\n");
    printf("     upper layer data stream with duplicate entries.\n");
    printf("3. Solution Implemented (Alternating Bit Protocol):\n");
    printf("   - The receiver maintains 'expected_seq' (0 or 1).\n");
    printf("   - Since expected_seq was 1 and the arriving frame had seq_no 0, the receiver:\n");
    printf("     a) Detected the duplicate.\n");
    printf("     b) Discarded the duplicate payload.\n");
    printf("     c) Re-sent ACK 0 to allow the sender to advance its sequence.\n");
    printf("4. Final Statistics: Unique frames accepted = %d, Duplicates discarded = %d\n",
           rx.total_accepted, rx.duplicate_count);
    printf("========================================================================================\n");

    return 0;
}
