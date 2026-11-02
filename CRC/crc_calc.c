/*
 * ============================================================================
 * CRC CALCULATION & ERROR DETECTION SIMULATION IN C
 * Description: Performs Modulo-2 polynomial long division (bitwise XOR),
 *              computes CRC remainder, builds transmitted codeword,
 *              simulates transmission noise (inverting leftmost bit),
 *              and demonstrates receiver error detection.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

/* Performs Modulo-2 XOR division on binary strings */
void modulo2_divide(const char *dividend, const char *divisor, char *remainder, char *quotient) {
    int dividend_len = strlen(dividend);
    int divisor_len = strlen(divisor);
    char cur[MAX_LEN];
    strcpy(cur, dividend);

    int q_idx = 0;
    for (int i = 0; i <= dividend_len - divisor_len; i++) {
        if (cur[i] == '1') {
            quotient[q_idx++] = '1';
            for (int j = 0; j < divisor_len; j++) {
                cur[i + j] = (cur[i + j] == divisor[j]) ? '0' : '1';
            }
        } else {
            quotient[q_idx++] = '0';
        }
    }
    quotient[q_idx] = '\0';

    // Remainder is the last (divisor_len - 1) bits
    int rem_len = divisor_len - 1;
    strncpy(remainder, cur + dividend_len - rem_len, rem_len);
    remainder[rem_len] = '\0';
}

/* Displays detailed step-by-step XOR long division */
void print_division_trace(const char *dividend, const char *divisor) {
    int dividend_len = strlen(dividend);
    int divisor_len = strlen(divisor);
    char cur[MAX_LEN];
    strcpy(cur, dividend);

    printf("  Initial Dividend : %s\n", cur);
    printf("  Generator Divisor: %s\n", divisor);
    printf("  ---------------------------------------------------\n");

    int step = 1;
    for (int i = 0; i <= dividend_len - divisor_len; i++) {
        if (cur[i] == '1') {
            printf("  Step %02d (Bit %02d = 1) -> XOR with Divisor:\n", step++, i);
            printf("    Current : %s\n", cur);
            printf("    Divisor : ");
            for (int k = 0; k < i; k++) printf(" ");
            printf("%s\n", divisor);

            for (int j = 0; j < divisor_len; j++) {
                cur[i + j] = (cur[i + j] == divisor[j]) ? '0' : '1';
            }

            printf("    Result  : %s\n\n", cur);
        }
    }

    int rem_len = divisor_len - 1;
    printf("  ---------------------------------------------------\n");
    printf("  Final Remainder (Last %d bits): %s\n", rem_len, cur + dividend_len - rem_len);
}

int main() {
    char message[MAX_LEN];
    char generator[MAX_LEN];
    char choice[16];

    printf("======================================================================\n");
    printf("          CYCLIC REDUNDANCY CHECK (CRC) SIMULATION IN C               \n");
    printf("======================================================================\n\n");

    printf("Choose an option:\n");
    printf("  1. Run with Alternate Message (110101101101, Generator: x^8+x^2+x+1)\n");
    printf("  2. Run with Original Message  (1011001001001011, Generator: x^8+x^2+x+1)\n");
    printf("  3. Enter custom Message and Generator\n");
    printf("Enter choice (1-3, default is 1): ");

    if (fgets(choice, sizeof(choice), stdin) == NULL || choice[0] == '\n' || choice[0] == '1') {
        strcpy(message, "110101101101");
        strcpy(generator, "100000111"); // x^8 + x^2 + x + 1
    } else if (choice[0] == '2') {
        strcpy(message, "1011001001001011");
        strcpy(generator, "100000111");
    } else {
        printf("\nEnter Data Message (binary string): ");
        if (scanf("%s", message) != 1) return 1;
        printf("Enter Generator Polynomial (binary string): ");
        if (scanf("%s", generator) != 1) return 1;
    }

    int deg_r = strlen(generator) - 1;

    // 1. Sender: Augment message with r zeros
    char augmented[MAX_LEN];
    strcpy(augmented, message);
    for (int i = 0; i < deg_r; i++) {
        strcat(augmented, "0");
    }

    printf("\n----------------------------------------------------------------------\n");
    printf("1. SENDER-SIDE CALCULATION\n");
    printf("----------------------------------------------------------------------\n");
    printf("Data Message (M)          : %s (Length: %d bits)\n", message, (int)strlen(message));
    printf("Generator Polynomial (G)  : %s (Degree r: %d)\n", generator, deg_r);
    printf("Augmented Message (M*2^r) : %s\n\n", augmented);

    printf("Performing Polynomial Long Division on Sender...\n\n");
    print_division_trace(augmented, generator);

    char remainder[MAX_LEN], quotient[MAX_LEN];
    modulo2_divide(augmented, generator, remainder, quotient);

    // 2. Transmitted codeword
    char transmitted[MAX_LEN];
    strcpy(transmitted, message);
    strcat(transmitted, remainder);

    printf("\n>> Calculated CRC Checksum (Remainder) : %s\n", remainder);
    printf(">> Transmitted Codeword T(x)            : %s\n", transmitted);

    // 3. Receiver Verification (No Error)
    printf("\n----------------------------------------------------------------------\n");
    printf("2. RECEIVER-SIDE VERIFICATION (NO ERROR)\n");
    printf("----------------------------------------------------------------------\n");
    char rx_rem_clean[MAX_LEN], rx_quot_clean[MAX_LEN];
    modulo2_divide(transmitted, generator, rx_rem_clean, rx_quot_clean);
    printf("Received Codeword : %s\n", transmitted);
    printf("Receiver Remainder: %s\n", rx_rem_clean);

    int is_clean_valid = 1;
    for (int i = 0; i < deg_r; i++) {
        if (rx_rem_clean[i] != '0') {
            is_clean_valid = 0;
            break;
        }
    }
    if (is_clean_valid) {
        printf("Result            : REMAINDER IS ALL ZEROS -> [DATA ACCEPTED, NO ERROR]\n");
    }

    // 4. Noise Simulation: Invert leftmost bit
    printf("\n----------------------------------------------------------------------\n");
    printf("3. NOISE SIMULATION: INVERTING LEFTMOST BIT\n");
    printf("----------------------------------------------------------------------\n");
    char corrupted[MAX_LEN];
    strcpy(corrupted, transmitted);
    corrupted[0] = (corrupted[0] == '1') ? '0' : '1';

    printf("Original Codeword  : %s\n", transmitted);
    printf("Corrupted Codeword : %s (Bit 0 inverted: %c -> %c)\n\n",
           corrupted, transmitted[0], corrupted[0]);

    printf("Performing Receiver Polynomial Long Division on Corrupted Frame...\n\n");
    print_division_trace(corrupted, generator);

    char rx_rem_err[MAX_LEN], rx_quot_err[MAX_LEN];
    modulo2_divide(corrupted, generator, rx_rem_err, rx_quot_err);

    printf("\n>> Receiver Remainder (Syndrome): %s\n", rx_rem_err);

    int has_error = 0;
    for (int i = 0; i < deg_r; i++) {
        if (rx_rem_err[i] != '0') {
            has_error = 1;
            break;
        }
    }

    if (has_error) {
        printf(">> Result: NON-ZERO REMAINDER (%s != 0)\n", rx_rem_err);
        printf(">> [ERROR DETECTED! Frame is discarded by Receiver.]\n");
    } else {
        printf(">> Result: Frame accepted.\n");
    }

    printf("======================================================================\n");
    return 0;
}
