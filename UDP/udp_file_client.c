/*
 * ============================================================================
 * UDP FILE TRANSFER - CLIENT
 * ============================================================================
 * Description:
 *   - Requests and downloads a file from the UDP file server.
 *   - Reconstructs incoming chunks and saves to "received_<filename>".
 *   - Configures a receive timeout so it detects if server stops responding.
 *   - Displays transfer statistics: chunk count, file size, integrity status.
 *
 * Compilation:
 *   - Windows (MinGW): gcc udp_file_client.c -o udp_file_client.exe -lws2_32
 *   - Linux / macOS  : gcc udp_file_client.c -o udp_file_client
 *
 * Execution:
 *   ./udp_file_client [server_ip] [server_port]
 *   Example: ./udp_file_client 127.0.0.1 8890
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
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
    #define SET_TIMEOUT(sockfd, timeout_sec) do { \
        struct timeval tv; \
        tv.tv_sec = timeout_sec; \
        tv.tv_usec = 0; \
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)); \
    } while(0)
#endif

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 8890
#define CHUNK_SIZE 1024

typedef struct {
    int seq_num;
    int bytes_in_chunk;
    int is_last;
    char data[CHUNK_SIZE];
} FilePacket;

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    const char *server_ip = (argc > 1) ? argv[1] : DEFAULT_SERVER_IP;
    int port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;
    char filename[256];

    INIT_NETWORK();

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Client] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    SET_TIMEOUT(client_fd, 4);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    printf("================================================================\n");
    printf("   UDP FILE TRANSFER CLIENT                                     \n");
    printf("   Target Server : %s:%d                                       \n", server_ip, port);
    printf("================================================================\n\n");

    printf("Enter filename to download from server (e.g. test.txt): ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        CLOSE_SOCKET(client_fd);
        CLEANUP_NETWORK();
        return 0;
    }
    filename[strcspn(filename, "\r\n")] = '\0';

    if (strlen(filename) == 0) {
        printf("[Client] Empty filename. Exiting.\n");
        CLOSE_SOCKET(client_fd);
        CLEANUP_NETWORK();
        return 0;
    }

    printf("[Client] Requesting \"%s\" from server...\n", filename);
    sendto(client_fd, filename, (int)strlen(filename), 0,
           (struct sockaddr *)&server_addr, addr_len);

    char output_filename[300];
    snprintf(output_filename, sizeof(output_filename), "downloaded_%s", filename);

    FILE *fp = NULL;
    FilePacket pkt;
    long total_received_bytes = 0;
    int chunk_count = 0;
    int transfer_success = 0;

    while (1) {
        memset(&pkt, 0, sizeof(FilePacket));
        int n = recvfrom(client_fd, (char *)&pkt, sizeof(FilePacket), 0,
                         (struct sockaddr *)&server_addr, &addr_len);

        if (n < 0) {
            printf("\n[Client] Timeout! Server stopped responding or packet was lost.\n");
            break;
        }

        if (pkt.seq_num == -1) {
            printf("\n[Client Error] Server says: File \"%s\" NOT FOUND on server!\n", filename);
            break;
        }

        if (fp == NULL) {
            fp = fopen(output_filename, "wb");
            if (fp == NULL) {
                perror("[Client] Failed to create local file");
                break;
            }
            printf("[Client] Downloading into: \"%s\"...\n", output_filename);
        }

        fwrite(pkt.data, 1, pkt.bytes_in_chunk, fp);
        total_received_bytes += pkt.bytes_in_chunk;
        chunk_count++;

        printf("  <- Received Chunk #%3d (%4d bytes) [EOF: %s]\n",
               pkt.seq_num, pkt.bytes_in_chunk, pkt.is_last ? "YES" : "NO");

        if (pkt.is_last) {
            transfer_success = 1;
            break;
        }
    }

    if (fp != NULL) {
        fclose(fp);
    }

    if (transfer_success) {
        printf("\n================================================================\n");
        printf("   FILE DOWNLOAD COMPLETED SUCCESSFULLY!                        \n");
        printf("   Saved as        : %s\n", output_filename);
        printf("   Total Chunks    : %d\n", chunk_count);
        printf("   Total Bytes     : %ld bytes\n", total_received_bytes);
        printf("================================================================\n");
    } else if (fp != NULL) {
        printf("[Client] Incomplete download. Partial file saved.\n");
    }

    CLOSE_SOCKET(client_fd);
    CLEANUP_NETWORK();
    return 0;
}
