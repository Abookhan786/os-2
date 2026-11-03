/*
 * ============================================================================
 * UDP FILE TRANSFER - SERVER
 * ============================================================================
 * Description:
 *   - Implements file transfer over connectionless UDP sockets (SOCK_DGRAM).
 *   - Listens on port 8890 for incoming file download requests from clients.
 *   - Reads the requested file in 1024-byte chunks and transmits them with
 *     packet sequence numbers and an End-Of-File (EOF) flag.
 *   - Supports Windows (MinGW/MSVC) and Linux/macOS.
 *
 * Compilation:
 *   - Windows (MinGW): gcc udp_file_server.c -o udp_file_server.exe -lws2_32
 *   - Linux / macOS  : gcc udp_file_server.c -o udp_file_server
 *
 * Execution:
 *   ./udp_file_server [optional_port]
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
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET(s) close(s)
    #define SLEEP_MS(ms) usleep((ms) * 1000)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
#endif

#define DEFAULT_PORT 8890
#define CHUNK_SIZE 1024

typedef struct {
    int seq_num;
    int bytes_in_chunk;
    int is_last;
    char data[CHUNK_SIZE];
} FilePacket;

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    char requested_filename[256];

    INIT_NETWORK();

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Server] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Server] Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("================================================================\n");
    printf("   UDP FILE TRANSFER SERVER                                     \n");
    printf("   Listening on Port : %d (UDP)                                 \n", port);
    printf("================================================================\n");
    printf("[Server] Waiting for client file requests...\n\n");

    while (1) {
        memset(requested_filename, 0, sizeof(requested_filename));

        int n = recvfrom(server_fd, requested_filename, sizeof(requested_filename) - 1, 0,
                         (struct sockaddr *)&client_addr, &client_len);
        if (n <= 0) continue;

        requested_filename[n] = '\0';
        requested_filename[strcspn(requested_filename, "\r\n")] = '\0';

        char *client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = ntohs(client_addr.sin_port);
        printf("[Request] Client [%s:%d] requested file: \"%s\"\n", client_ip, client_port, requested_filename);

        FILE *fp = fopen(requested_filename, "rb");
        if (fp == NULL) {
            printf("[Server] File \"%s\" not found. Sending error notification.\n", requested_filename);
            FilePacket err_pkt;
            err_pkt.seq_num = -1;
            err_pkt.bytes_in_chunk = 0;
            err_pkt.is_last = 1;
            strcpy(err_pkt.data, "FILE_NOT_FOUND");
            sendto(server_fd, (char *)&err_pkt, sizeof(FilePacket), 0,
                   (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        printf("[Server] File found! Size: %ld bytes. Transmitting via UDP...\n", file_size);

        FilePacket pkt;
        int seq = 0;
        long total_sent_bytes = 0;

        while (!feof(fp)) {
            memset(&pkt, 0, sizeof(FilePacket));
            pkt.seq_num = ++seq;

            int bytes_read = (int)fread(pkt.data, 1, CHUNK_SIZE, fp);
            pkt.bytes_in_chunk = bytes_read;
            pkt.is_last = (feof(fp) || bytes_read < CHUNK_SIZE) ? 1 : 0;

            sendto(server_fd, (char *)&pkt, sizeof(FilePacket), 0,
                   (struct sockaddr *)&client_addr, client_len);

            total_sent_bytes += bytes_read;
            printf("  -> Sent Chunk #%3d (%4d bytes) [EOF: %s]\n",
                   pkt.seq_num, pkt.bytes_in_chunk, pkt.is_last ? "YES" : "NO");

            SLEEP_MS(5);

            if (pkt.is_last) break;
        }

        fclose(fp);
        printf("[Server] File transfer complete: %ld bytes sent across %d chunks to [%s:%d]\n\n",
               total_sent_bytes, seq, client_ip, client_port);
    }

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
