/*
 * ============================================================================
 * GO-BACK-N (GBN) ARQ PROTOCOL USING UDP SOCKETS - RECEIVER
 * ============================================================================
 * Description:
 *   - Implements the Receiver side of Go-Back-N ARQ over UDP (SOCK_DGRAM).
 *   - Listens on port 8891 for packets from the GBN sender.
 *   - Only accepts frames in sequential order (expected_seq).
 *   - Discards any out-of-order frames and sends a duplicate cumulative ACK
 *     for the highest in-order frame received.
 *   - Allows interactive or automatic simulation of packet drops to trigger
 *     sender timeout and Go-Back-N retransmission.
 *
 * Compilation:
 *   - Windows (MinGW): gcc gbn_receiver.c -o gbn_receiver.exe -lws2_32
 *   - Linux / macOS  : gcc gbn_receiver.c -o gbn_receiver
 *
 * Execution:
 *   ./gbn_receiver [optional_port]
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

#define DEFAULT_PORT 8891

typedef struct {
    int seq_num;
    char data[64];
} GBNPacket;

typedef struct {
    int ack_num;
} GBNAck;

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
    printf("   GO-BACK-N (GBN) ARQ RECEIVER (UDP Socket)                    \n");
    printf("   Listening on Port : %d                                      \n", port);
    printf("================================================================\n\n");

    int drop_packet_target = -1;
    char input_buf[32];
    printf("Simulate dropping a packet to test Go-Back-N? \n");
    printf("Enter packet number to drop on first arrival (e.g. 2, or -1 for no loss): ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        drop_packet_target = atoi(input_buf);
    }

    if (drop_packet_target >= 0) {
        printf("[Simulation] Packet %d will be dropped once to trigger Go-Back-N retransmission.\n\n",
               drop_packet_target);
    } else {
        printf("[Simulation] Clean channel: No intentional drops configured.\n\n");
    }

    printf("[Receiver] Waiting for frames from sender...\n\n");

    int expected_seq = 0;
    int drop_simulated = 0;
    GBNPacket packet;
    GBNAck ack;

    while (1) {
        memset(&packet, 0, sizeof(GBNPacket));
        int n = recvfrom(server_fd, (char *)&packet, sizeof(GBNPacket), 0,
                         (struct sockaddr *)&client_addr, &client_len);
        if (n <= 0) continue;

        if (packet.seq_num == -1) {
            printf("\n[Receiver] Termination signal received from sender. Transfer complete!\n");
            break;
        }

        printf("----------------------------------------------------------------\n");
        printf("[Receiver] Received Frame #%d: \"%s\"\n", packet.seq_num, packet.data);

        if (packet.seq_num == drop_packet_target && !drop_simulated) {
            printf(">>> [SIMULATED DROP] Packet %d intentionally dropped!\n", packet.seq_num);
            printf("    (No ACK sent. Sender will time out and Go-Back-N)\n");
            drop_simulated = 1;
            continue;
        }

        if (packet.seq_num == expected_seq) {

            printf("    -> Status: IN-ORDER. Accepted!\n");
            ack.ack_num = expected_seq;
            sendto(server_fd, (char *)&ack, sizeof(GBNAck), 0,
                   (struct sockaddr *)&client_addr, client_len);
            printf("    <- Sent Cumulative ACK %d to Sender\n", ack.ack_num);
            expected_seq++;
        } else {

            printf("    -> Status: OUT-OF-ORDER (Expected %d, got %d). DISCARDED!\n",
                   expected_seq, packet.seq_num);
            ack.ack_num = expected_seq - 1;
            sendto(server_fd, (char *)&ack, sizeof(GBNAck), 0,
                   (struct sockaddr *)&client_addr, client_len);
            printf("    <- Resent Duplicate ACK %d to Sender\n", ack.ack_num);
        }
    }

    printf("================================================================\n");
    printf("   RECEIVER FINISHED SUCCESSFULLY                               \n");
    printf("================================================================\n");

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
