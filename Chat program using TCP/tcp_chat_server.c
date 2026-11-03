/*
 * ============================================================================
 * MODEL QUESTION 1: TCP CHAT SERVER (C LANGUAGE)
 * Description: Interactive two-way connection-oriented Chat Server using TCP.
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

#define PORT 8889
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    INIT_NETWORK();

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

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

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("====================================================\n");
    printf("         TCP CHAT SERVER (Port: %d)                 \n", PORT);
    printf("   Waiting for incoming connection from client...   \n");
    printf("====================================================\n\n");

    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("[SERVER]: Client connected successfully from %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    printf("[SERVER]: You can now chat! Type 'exit' to disconnect.\n\n");

    while (1) {

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            printf("\n[SERVER]: Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("[Client]: %s\n", buffer);

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[SERVER]: Client ended the chat session.\n");
            break;
        }

        printf("Server: ");
        memset(buffer, 0, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (send(client_fd, buffer, strlen(buffer), 0) < 0) {
            perror("send error");
            break;
        }

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[SERVER]: You terminated the chat.\n");
            break;
        }
    }

    CLOSE_SOCKET(client_fd);
    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    printf("Server shutdown complete.\n");
    return 0;
}
