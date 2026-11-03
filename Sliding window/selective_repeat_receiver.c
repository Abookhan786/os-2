/*
 * ============================================================================
 * SELECTIVE REPEAT (SR) ARQ PROTOCOL USING UDP SOCKETS - RECEIVER
 * ============================================================================
 * Description:
 *   - Implements the Receiver side of Selective Repeat ARQ over UDP.
 *   - Maintains a Receive Window [rcv_base, rcv_base + WINDOW_SIZE - 1].
 *   - Key Feature: OUT-OF-ORDER PACKETS ARE BUFFERED (not discarded as in GBN).
 *   - Replies with an INDIVIDUAL (Selective) ACK for every packet received.
 *   - Once the base packet arrives, all consecutively buffered packets are
 *     delivered in order, and the receive window slides forward.
 *   - Allows simulating an intentional packet drop.
 *
 * Compilation:
 *   - Windows (MinGW): gcc selective_repeat_receiver.c -o selective_repeat_receiver.exe -lws2_32
 *   - Linux / macOS  : gcc selective_repeat_receiver.c -o selective_repeat_receiver
 *
 * Execution:
 *   ./selective_repeat_receiver [optional_port]
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
    typedef int socklen_t;
    #define CLOSE_SOCKET(s) closesocket(s)
    #define INIT_NETWORK() do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while(0)
    #define CLEANUP_NETWORK() WSACleanup()
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET(s) close(s)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
#endif

#define DEFAULT_PORT 8892
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 8

typedef struct {
    int seq_num;
    char data[64];
} SRPacket;

typedef struct {
    int ack_num;
} SRAck;

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;

    INIT_NETWORK();

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Receiver] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Receiver] Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("================================================================\n");
    printf("   SELECTIVE REPEAT (SR) ARQ RECEIVER (UDP Socket)              \n");
    printf("   Window Size (W)   : %d                                       \n", WINDOW_SIZE);
    printf("   Listening on Port : %d                                       \n", port);
    printf("================================================================\n\n");

    int drop_packet_target = -1;
    char input_buf[32];
    printf("Simulate dropping a packet to test Selective Repeat? \n");
    printf("Enter packet number to drop on first arrival (e.g. 2, or -1 for no loss): ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        drop_packet_target = atoi(input_buf);
    }

    if (drop_packet_target >= 0) {
        printf("[Simulation] Packet %d will be dropped on first arrival to test buffering & selective ACK.\n\n",
               drop_packet_target);
    } else {
        printf("[Simulation] Clean channel: No intentional drops configured.\n\n");
    }

    int rcv_base = 0;
    int received[TOTAL_PACKETS] = {0};
    char buffer_pool[TOTAL_PACKETS][64];
    int drop_simulated = 0;

    SRPacket packet;
    SRAck ack;

    printf("[Receiver] Waiting for frames from sender...\n\n");

    while (rcv_base < TOTAL_PACKETS) {
        memset(&packet, 0, sizeof(SRPacket));
        int n = recvfrom(server_fd, (char *)&packet, sizeof(SRPacket), 0,
                         (struct sockaddr *)&client_addr, &client_len);
        if (n <= 0) continue;

        if (packet.seq_num == -1) {
            printf("[Receiver] Sender indicated completion.\n");
            break;
        }

        int seq = packet.seq_num;
        printf("----------------------------------------------------------------\n");
        printf("[Receiver] Received Frame #%d: \"%s\"\n", seq, packet.data);

        if (seq == drop_packet_target && !drop_simulated) {
            printf(">>> [SIMULATED DROP] Packet %d dropped once!\n", seq);
            printf("    (No ACK sent. Sender will time out and retransmit ONLY Packet %d)\n", seq);
            drop_simulated = 1;
            continue;
        }

        if (seq >= rcv_base && seq < rcv_base + WINDOW_SIZE && seq < TOTAL_PACKETS) {

            ack.ack_num = seq;
            sendto(server_fd, (char *)&ack, sizeof(SRAck), 0,
                   (struct sockaddr *)&client_addr, client_len);
            printf("    <- Sent SELECTIVE ACK for Packet %d\n", seq);

            if (!received[seq]) {
                received[seq] = 1;
                strncpy(buffer_pool[seq], packet.data, sizeof(buffer_pool[seq]) - 1);

                if (seq == rcv_base) {

                    printf("    -> Packet %d is at base! Delivering in-order:\n", seq);
                    while (rcv_base < TOTAL_PACKETS && received[rcv_base]) {
                        printf("       * Delivered Packet %d to Application Layer: \"%s\"\n",
                               rcv_base, buffer_pool[rcv_base]);
                        rcv_base++;
                    }
                    printf("    -> Receive Window slid forward. New rcv_base = %d\n", rcv_base);
                } else {
                    printf("    -> Out-of-order Packet %d! BUFFERED in receive window.\n", seq);
                }
            } else {
                printf("    -> Packet %d was already buffered/received.\n", seq);
            }
        }

        else if (seq < rcv_base) {
            ack.ack_num = seq;
            sendto(server_fd, (char *)&ack, sizeof(SRAck), 0,
                   (struct sockaddr *)&client_addr, client_len);
            printf("    <- Re-sent ACK for already delivered Packet %d\n", seq);
        } else {
            printf("    -> Packet %d outside receive window. Ignored.\n", seq);
        }
    }

    printf("\n================================================================\n");
    printf("   SELECTIVE REPEAT RECEIVER COMPLETED                          \n");
    printf("   All %d packets received, buffered, and delivered in order!   \n", TOTAL_PACKETS);
    printf("================================================================\n");

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
