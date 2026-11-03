/*
 * ============================================================================
 * GO-BACK-N (GBN) ARQ PROTOCOL USING UDP SOCKETS - SENDER
 * ============================================================================
 * Description:
 *   - Implements a Pipelined Sliding Window Go-Back-N ARQ over UDP (SOCK_DGRAM).
 *   - Transmits packets up to WINDOW_SIZE without waiting for individual ACKs.
 *   - Uses cumulative ACKs and socket timeout detection.
 *   - On timeout of the oldest unacknowledged packet (base), it GOES BACK N:
 *     retransmits ALL packets currently in the window starting from base.
 *   - Cross-platform: Compiles on Windows (-lws2_32) and Linux/macOS.
 *
 * Compilation:
 *   - Windows (MinGW): gcc gbn_sender.c -o gbn_sender.exe -lws2_32
 *   - Linux / macOS  : gcc gbn_sender.c -o gbn_sender
 *
 * Execution:
 *   Run receiver first, then run sender:
 *   ./gbn_sender [receiver_ip] [receiver_port]
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
    #define SLEEP_MS(ms) Sleep(ms)
    #define INIT_NETWORK() do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while(0)
    #define CLEANUP_NETWORK() WSACleanup()
    #define SET_TIMEOUT(sockfd, timeout_sec) do { \
        DWORD tv = (timeout_sec) * 1000; \
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); \
    } while(0)
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET(s) close(s)
    #define SLEEP_MS(ms) usleep((ms) * 1000)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
    #define SET_TIMEOUT(sockfd, timeout_sec) do { \
        struct timeval tv; \
        tv.tv_sec = timeout_sec; \
        tv.tv_usec = 0; \
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); \
    } while(0)
#endif

#define DEFAULT_RECEIVER_IP "127.0.0.1"
#define DEFAULT_PORT 8891
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 8
#define TIMEOUT_SEC 2

typedef struct {
    int seq_num;
    char data[64];
} GBNPacket;

typedef struct {
    int ack_num;
} GBNAck;

void print_window(int base, int next_seq) {
    printf("  [Window: [");
    for (int i = 0; i < TOTAL_PACKETS; i++) {
        if (i == base) printf("| ");
        if (i >= base && i < next_seq) {
            printf("P%d* ", i);
        } else if (i >= next_seq && i < base + WINDOW_SIZE) {
            printf("P%d  ", i);
        } else {
            printf("P%d  ", i);
        }
        if (i == base + WINDOW_SIZE - 1) printf("| ");
    }
    printf("] Base=%d, Next=%d]\n", base, next_seq);
}

int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_in receiver_addr;
    socklen_t addr_len = sizeof(receiver_addr);
    const char *receiver_ip = (argc > 1) ? argv[1] : DEFAULT_RECEIVER_IP;
    int port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;

    INIT_NETWORK();

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Sender] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    SET_TIMEOUT(sock_fd, TIMEOUT_SEC);

    memset(&receiver_addr, 0, sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(port);
    receiver_addr.sin_addr.s_addr = inet_addr(receiver_ip);

    printf("   GO-BACK-N (GBN) ARQ SENDER (UDP Socket)                      \n");
    printf("   Window Size (N) : %d                                         \n", WINDOW_SIZE);
    printf("   Total Packets   : %d                                         \n", TOTAL_PACKETS);
    printf("   Timeout Period  : %d seconds                                 \n", TIMEOUT_SEC);

    int base = 0;
    int next_seq = 0;
    GBNPacket packet;
    GBNAck ack;

    while (base < TOTAL_PACKETS) {

        while (next_seq < base + WINDOW_SIZE && next_seq < TOTAL_PACKETS) {
            packet.seq_num = next_seq;
            snprintf(packet.data, sizeof(packet.data), "Payload of Packet %d", next_seq);

            printf("[Sender] Transmitting Packet %d: \"%s\"\n", packet.seq_num, packet.data);
            sendto(sock_fd, (char *)&packet, sizeof(GBNPacket), 0,
                   (struct sockaddr *)&receiver_addr, addr_len);

            next_seq++;
            print_window(base, next_seq);
            SLEEP_MS(50);
        }

        memset(&ack, 0, sizeof(GBNAck));
        int n = recvfrom(sock_fd, (char *)&ack, sizeof(GBNAck), 0,
                         (struct sockaddr *)&receiver_addr, &addr_len);

        if (n >= 0) {

            printf("\n<<< [Sender] Received Cumulative ACK %d from Receiver\n", ack.ack_num);

            if (ack.ack_num >= base) {
                base = ack.ack_num + 1;
                printf("    Window slides forward! New Base = %d\n", base);
                print_window(base, next_seq);
            } else {
                printf("    Duplicate ACK %d received (ignored).\n", ack.ack_num);
            }
            printf("\n");
        } else {

            printf("\n****************************************************************\n");
            printf(">>> [TIMEOUT] Timer expired for Packet %d!\n", base);
            printf(">>> [GO-BACK-N] Retransmitting entire window from %d to %d...\n",
                   base, next_seq - 1);
            printf("****************************************************************\n\n");

            next_seq = base;
        }
    }

    packet.seq_num = -1;
    strcpy(packet.data, "TERMINATE");
    sendto(sock_fd, (char *)&packet, sizeof(GBNPacket), 0,
           (struct sockaddr *)&receiver_addr, addr_len);

    printf("\n================================================================\n");
    printf("   GO-BACK-N TRANSMISSION COMPLETE!                             \n");
    printf("   All %d packets transmitted and acknowledged successfully.    \n", TOTAL_PACKETS);
    printf("================================================================\n");

    CLOSE_SOCKET(sock_fd);
    CLEANUP_NETWORK();
    return 0;
}
