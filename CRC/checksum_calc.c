/*
 * ============================================================================
 * INTERNET CHECKSUM (16-BIT 1'S COMPLEMENT) CALCULATION & VERIFICATION IN C
 * ============================================================================
 * Description:
 *   - Implements the standard Internet Checksum algorithm (RFC 1071) used in
 *     IPv4, TCP, and UDP headers.
 *   - Sender Side:
 *       1. Divides the data into 16-bit words (pads trailing odd byte with 0).
 *       2. Computes the 1's complement sum (summing into a 32-bit accumulator
 *          and folding the overflow carry bits back into the lower 16 bits).
 *       3. Takes the bitwise NOT (1's complement) of the sum to produce the Checksum.
 *       4. Transmits [Data + Checksum].
 *   - Receiver Side:
 *       1. Sums all 16-bit data words PLUS the received checksum.
 *       2. Folds carry bits.
 *       3. If the final sum equals 0xFFFF (bitwise NOT is 0x0000), packet is VALID.
 *       4. Otherwise, ERROR IS DETECTED and packet is marked corrupted.
 *   - Simulates transmission error injection (bit-flip / character tampering).
 *
 * Compilation:
 *   gcc checksum_calc.c -o checksum_calc.exe
 *
 * Execution:
 *   ./checksum_calc.exe
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_BUFFER 512

void to_binary16(uint16_t val, char *str) {
    for (int i = 15; i >= 0; i--) {
        *str++ = ((val >> i) & 1) ? '1' : '0';
        if (i % 4 == 0 && i != 0) *str++ = ' ';
    }
    *str = '\0';
}

uint16_t compute_checksum(const uint8_t *buf, int len, int print_steps) {
    uint32_t sum = 0;
    int words = len / 2;
    char bin_str[32];

    if (print_steps) {
        printf("\n--- Step-by-Step 16-bit Addition ---\n");
    }

    for (int i = 0; i < words; i++) {

        uint16_t word = ((uint16_t)buf[2 * i] << 8) | (uint16_t)buf[2 * i + 1];
        uint32_t prev_sum = sum;
        sum += word;

        if (print_steps) {
            to_binary16(word, bin_str);
            printf("  Word %2d: 0x%04X | %s ('%c%c')\n",
                   i + 1, word, bin_str, buf[2 * i], buf[2 * i + 1]);
        }
    }

    if (len % 2 != 0) {
        uint16_t word = ((uint16_t)buf[len - 1] << 8);
        sum += word;
        if (print_steps) {
            to_binary16(word, bin_str);
            printf("  Odd Byte: 0x%04X | %s ('%c' + padded 0)\n",
                   word, bin_str, buf[len - 1]);
        }
    }

    if (print_steps) {
        printf("------------------------------------\n");
        printf("  Accumulated Sum before carry fold : 0x%08X\n", sum);
    }

    while (sum >> 16) {
        if (print_steps) {
            printf("  Adding carry 0x%04X to lower 16 bits 0x%04X...\n",
                   (uint16_t)(sum >> 16), (uint16_t)(sum & 0xFFFF));
        }
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    if (print_steps) {
        to_binary16((uint16_t)sum, bin_str);
        printf("  Final Folded Sum                  : 0x%04X | %s\n", (uint16_t)sum, bin_str);
    }

    uint16_t checksum = ~((uint16_t)sum);

    if (print_steps) {
        to_binary16(checksum, bin_str);
        printf("  Checksum (~Sum)                   : 0x%04X | %s\n", checksum, bin_str);
        printf("------------------------------------\n");
    }

    return checksum;
}

int verify_checksum(const uint8_t *buf, int len, uint16_t received_checksum) {
    uint32_t sum = 0;
    int words = len / 2;

    for (int i = 0; i < words; i++) {
        uint16_t word = ((uint16_t)buf[2 * i] << 8) | (uint16_t)buf[2 * i + 1];
        sum += word;
    }

    if (len % 2 != 0) {
        sum += ((uint16_t)buf[len - 1] << 8);
    }

    sum += received_checksum;

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    uint16_t final_sum = (uint16_t)sum;
    uint16_t result = ~final_sum;

    printf("  Receiver Computed Sum (Data + Checksum) : 0x%04X\n", final_sum);
    printf("  Receiver 1's Complement (~Sum)          : 0x%04X\n", result);

    return (result == 0);
}

int main() {
    char message[MAX_BUFFER];

    printf("================================================================\n");
    printf("   INTERNET CHECKSUM (RFC 1071) SIMULATION IN C                 \n");
    printf("================================================================\n\n");

    printf("Enter text message to transmit (e.g. \"Networks Lab\"): ");
    if (fgets(message, sizeof(message), stdin) == NULL) {
        return 0;
    }
    message[strcspn(message, "\r\n")] = '\0';

    if (strlen(message) == 0) {
        strcpy(message, "Networks Lab");
        printf("Using default message: \"%s\"\n", message);
    }

    int len = (int)strlen(message);
    printf("\nOriginal Message : \"%s\" (%d bytes)\n", message, len);

    printf("\n================================================================\n");
    printf(" [1. SENDER SIDE] COMPUTING CHECKSUM                            \n");
    printf("================================================================\n");
    uint16_t sender_checksum = compute_checksum((const uint8_t *)message, len, 1);

    printf("\n>>> Generated Checksum to transmit : 0x%04X\n", sender_checksum);

    printf("\n================================================================\n");
    printf(" [2. RECEIVER SIDE - CASE A] ERROR-FREE TRANSMISSION            \n");
    printf("================================================================\n");
    printf("Simulating clean channel transmission...\n");
    int valid = verify_checksum((const uint8_t *)message, len, sender_checksum);

    if (valid) {
        printf(">>> [RESULT: ACCEPTED] Checksum Validated! No error detected.\n");
        printf("    Extracted Data: \"%s\"\n", message);
    } else {
        printf(">>> [RESULT: ERROR DETECTED] Packet corrupted!\n");
    }

    printf("\n================================================================\n");
    printf(" [3. RECEIVER SIDE - CASE B] ERROR DETECTION DEMONSTRATION      \n");
    printf("================================================================\n");
    char corrupted_message[MAX_BUFFER];
    strcpy(corrupted_message, message);

    corrupted_message[0] ^= 0x01;
    printf("Simulating transmission noise: Altered first character from '%c' to '%c'\n",
           message[0], corrupted_message[0]);
    printf("Corrupted Received Data: \"%s\"\n", corrupted_message);

    valid = verify_checksum((const uint8_t *)corrupted_message, len, sender_checksum);

    if (valid) {
        printf(">>> [RESULT: ACCEPTED] (Unexpected) Checksum reported clean.\n");
    } else {
        printf(">>> [RESULT: REJECTED] CHECKSUM MISMATCH DETECTED!\n");
        printf("    Packet is corrupt and will be discarded by receiver.\n");
    }

    printf("\n================================================================\n");
    printf("   CHECKSUM DEMONSTRATION COMPLETE                              \n");
    printf("================================================================\n");

    return 0;
}
