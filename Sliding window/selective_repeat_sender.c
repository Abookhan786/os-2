/*
 * ============================================================================
 * SELECTIVE REPEAT (SR) ARQ PROTOCOL USING UDP SOCKETS - SENDER
 * ============================================================================
 * Description:
 *   - Implements a Pipelined Sliding Window with Selective Repeat ARQ over UDP.
 *   - Unlike Go-Back-N, the sender receives individual (selective) ACKs for each
 *     specific frame.
 *   - On timeout, the sender retransmits ONLY the specific unacknowledged packet,
 *     not the whole window, saving substantial bandwidth.
 *   - Window base only advances when the packet at base is acknowledged.
 *   - Cross-platform: Compiles on Windows (-lws2_32) and Linux/macOS.
 *
 * Compilation:
 *   - Windows (MinGW): gcc selective_repeat_sender.c -o selective_repeat_sender.exe -lws2_32
 *   - Linux / macOS  : gcc selective_repeat_sender.c -o selective_repeat_sender
 *
 * Execution:
 *   Run receiver first, then run sender:
 *   ./selective_repeat_sender [receiver_ip] [receiver_port]
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
#define DEFAULT_PORT 8892
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 8
#define TIMEOUT_SEC 2

typedef struct {
    int seq_num;
    char data[64];
} SRPacket;

typedef struct {
    int ack_num;
} SRAck;

void print_sr_window(int base, const int *acked, const int *sent) {
    printf("  [Sender Window: ");
    for (int i = 0; i < TOTAL_PACKETS; i++) {
        if (i == base) printf("[ ");
        if (acked[i]) {
            printf("P%d(ACK) ", i);
        } else if (sent[i]) {
            printf("P%d(Sent) ", i);
        } else if (i < base + WINDOW_SIZE) {
            printf("P%d(Ready) ", i);
        } else {
            printf("P%d ", i);
        }
        if (i == base + WINDOW_SIZE - 1) printf("] ");
    }
    printf("Base=%d]\n", base);
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

    printf("================================================================\n");
    printf("   SELECTIVE REPEAT (SR) ARQ SENDER (UDP Socket)                \n");
    printf("   Window Size (W) : %d                                         \n", WINDOW_SIZE);
    printf("   Total Packets   : %d                                         \n", TOTAL_PACKETS);
    printf("   Timeout Period  : %d seconds                                 \n", TIMEOUT_SEC);
    printf("================================================================\n\n");

    int base = 0;
    int acked[TOTAL_PACKETS] = {0};
    int sent[TOTAL_PACKETS] = {0};

    SRPacket packet;
    SRAck ack;

    while (base < TOTAL_PACKETS) {

        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++) {
            if (!sent[i] && !acked[i]) {
                packet.seq_num = i;
                snprintf(packet.data, sizeof(packet.data), "SR-Data-Payload #%d", i);

                printf("[Sender] Transmitting Packet %d: \"%s\"\n", packet.seq_num, packet.data);
                sendto(sock_fd, (char *)&packet, sizeof(SRPacket), 0,
                       (struct sockaddr *)&receiver_addr, addr_len);

                sent[i] = 1;
                print_sr_window(base, acked, sent);
                SLEEP_MS(50);
            }
        }

        memset(&ack, 0, sizeof(SRAck));
        int n = recvfrom(sock_fd, (char *)&ack, sizeof(SRAck), 0,
                         (struct sockaddr *)&receiver_addr, &addr_len);

        if (n >= 0) {
            int a = ack.ack_num;
            printf("\n<<< [Sender] Received SELECTIVE ACK for Packet %d\n", a);

            if (a >= 0 && a < TOTAL_PACKETS) {
                acked[a] = 1;

                if (a == base) {
                    while (base < TOTAL_PACKETS && acked[base]) {
                        base++;
                    }
                    printf("    Base acknowledged! Window slides forward. New Base = %d\n", base);
                }
                print_sr_window(base, acked, sent);
            }
            printf("\n");
        } else {

            printf("\n****************************************************************\n");
            printf(">>> [TIMEOUT] Timer expired! Locating unacknowledged packet(s)...\n");

            for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++) {
                if (!acked[i]) {
                    printf(">>> [SELECTIVE REPEAT] Retransmitting ONLY Packet %d (others spared)\n", i);
                    packet.seq_num = i;
                    snprintf(packet.data, sizeof(packet.data), "SR-Data-Payload #%d", i);

                    sendto(sock_fd, (char *)&packet, sizeof(SRPacket), 0,
                           (struct sockaddr *)&receiver_addr, addr_len);
                    break;
                }
            }
            printf("****************************************************************\n\n");
        }
    }

    packet.seq_num = -1;
    strcpy(packet.data, "TERMINATE");
    sendto(sock_fd, (char *)&packet, sizeof(SRPacket), 0,
           (struct sockaddr *)&receiver_addr, addr_len);

    printf("\n================================================================\n");
    printf("   SELECTIVE REPEAT TRANSMISSION COMPLETE!                      \n");
    printf("   All %d packets successfully transferred with individual ACKs.\n", TOTAL_PACKETS);
    printf("================================================================\n");

    CLOSE_SOCKET(sock_fd);
    CLEANUP_NETWORK();
    return 0;
}
