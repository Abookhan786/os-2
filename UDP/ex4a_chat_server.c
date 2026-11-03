/*
 * ============================================================================
 * EX. NO: 4a - UDP SOCKET PROGRAMMING (CHAT SERVER)
 * Description: Two-way interactive Chat Server using UDP Sockets in C.
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

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    INIT_NETWORK();

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("====================================================\n");
    printf("        UDP CHAT SERVER STARTED (Port: %d)          \n", PORT);
    printf("  Waiting for client message... (Type 'exit' to quit)\n");
    printf("====================================================\n\n");

    while (1) {

        memset(buffer, 0, BUFFER_SIZE);

        int bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("recvfrom error");
            break;
        }

        buffer[bytes_received] = '\0';

        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("[Client %s:%d]: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[Server]: Client has left the chat. Shutting down...\n");
            break;
        }

        printf("Server: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (sendto(server_fd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("sendto error");
            break;
        }

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[Server]: You terminated the chat.\n");
            break;
        }
    }

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    printf("Server socket closed.\n");
    return 0;
}
