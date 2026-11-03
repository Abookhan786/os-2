/*
 * ============================================================================
 * DNS CLIENT USING UDP SOCKETS IN C
 * ============================================================================
 * Description:
 *   - Simulates a Domain Name System (DNS) Client over UDP (SOCK_DGRAM).
 *   - Sends domain name queries (e.g. "www.google.com") or IP addresses
 *     (for reverse lookup) to the DNS server.
 *   - Receives and displays the resolved records cleanly.
 *   - Configures a socket receive timeout to prevent hanging if the server
 *     is unreachable.
 *   - Supports Windows (MinGW/MSVC) and Linux/macOS seamlessly.
 *
 * Compilation:
 *   - Windows (MinGW): gcc dns_client.c -o dns_client -lws2_32
 *   - Linux / macOS  : gcc dns_client.c -o dns_client
 *
 * Execution:
 *   ./dns_client [server_ip] [server_port]
 *   Example: ./dns_client 127.0.0.1 8053
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
#define DEFAULT_PORT 8053
#define BUFFER_SIZE 1024

void display_response(const char *response) {
    char temp[BUFFER_SIZE];
    strncpy(temp, response, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char *status = strtok(temp, "|");
    if (status != NULL && strcmp(status, "SUCCESS") == 0) {
        char *type = strtok(NULL, "|");
        char *query = strtok(NULL, "|");
        char *result = strtok(NULL, "|");

        printf("----------------------------------------------------------------\n");
        printf(" [DNS RESPONSE: SUCCESS]\n");
        if (strcmp(type, "FORWARD") == 0) {
            printf("  Lookup Type : Forward DNS (Domain -> IP)\n");
            printf("  Domain Name : %s\n", query);
            printf("  IP Address  : %s\n", result);
        } else if (strcmp(type, "REVERSE") == 0) {
            printf("  Lookup Type : Reverse DNS (IP -> Domain)\n");
            printf("  IP Address  : %s\n", query);
            printf("  Domain Name : %s\n", result);
        } else if (strcmp(type, "LIVE_RESOLVED") == 0) {
            printf("  Lookup Type : Live External DNS Resolution\n");
            printf("  Domain Name : %s\n", query);
            printf("  IP Address  : %s\n", result);
        }
        printf("----------------------------------------------------------------\n\n");
    } else if (status != NULL && strcmp(status, "ERROR") == 0) {
        char *error_msg = strtok(NULL, "");
        printf("----------------------------------------------------------------\n");
        printf(" [DNS RESPONSE: FAILED]\n");
        printf("  Reason: %s\n", (error_msg ? error_msg : "Unknown error"));
        printf("----------------------------------------------------------------\n\n");
    } else {
        printf(" [Raw Response]: %s\n\n", response);
    }
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char query[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    const char *server_ip = (argc > 1) ? argv[1] : DEFAULT_SERVER_IP;
    int server_port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;

    INIT_NETWORK();

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Client] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    SET_TIMEOUT(client_fd, 4);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    if (server_addr.sin_addr.s_addr == INADDR_NONE && strcmp(server_ip, "255.255.255.255") != 0) {
        perror("[Client] Invalid server IP address format");
        CLOSE_SOCKET(client_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("================================================================\n");
    printf("   DNS CLIENT (UDP Socket Simulation)                          \n");
    printf("   Connected to DNS Server : %s:%d                             \n", server_ip, server_port);
    printf("   Type a domain name or IP address to resolve.                \n");
    printf("   Type 'exit' or 'quit' to terminate.                         \n");
    printf("================================================================\n\n");

    while (1) {
        printf("Enter Domain Name or IP: ");
        if (fgets(query, sizeof(query), stdin) == NULL) {
            break;
        }

        query[strcspn(query, "\r\n")] = '\0';

        if (strlen(query) == 0) {
            continue;
        }

        if (strcmp(query, "exit") == 0 || strcmp(query, "quit") == 0) {

            sendto(client_fd, query, (int)strlen(query), 0,
                   (struct sockaddr *)&server_addr, addr_len);
            printf("[Client] Exiting...\n");
            break;
        }

        int bytes_sent = sendto(client_fd, query, (int)strlen(query), 0,
                                (struct sockaddr *)&server_addr, addr_len);
        if (bytes_sent < 0) {
            perror("[Client] Failed to send query");
            continue;
        }

        memset(response, 0, sizeof(response));
        int bytes_received = recvfrom(client_fd, response, sizeof(response) - 1, 0,
                                      (struct sockaddr *)&server_addr, &addr_len);
        if (bytes_received < 0) {
            printf("[Client] Timeout! No response received from DNS Server (%s:%d).\n",
                   server_ip, server_port);
            printf("         (Check if the server is running)\n\n");
            continue;
        }

        response[bytes_received] = '\0';

        display_response(response);
    }

    CLOSE_SOCKET(client_fd);
    CLEANUP_NETWORK();
    return 0;
}
