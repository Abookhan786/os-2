/*
 * ============================================================================
 * MODEL QUESTION 1: TCP CHAT CLIENT (C LANGUAGE)
 * Description: Interactive two-way connection-oriented Chat Client using TCP.
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

#define SERVER_IP "127.0.0.1"
#define PORT 8889
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char *target_ip = (argc > 1) ? argv[1] : SERVER_IP;

    INIT_NETWORK();

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    printf("====================================================\n");
    printf("         TCP CHAT CLIENT CONNECTING...              \n");
    printf("         Target Server: %s:%d                       \n", target_ip, PORT);
    printf("====================================================\n\n");

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed. Make sure server is running");
        CLOSE_SOCKET(client_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT]: Connected successfully to server!\n");
    printf("[CLIENT]: Type message and press Enter ('exit' to quit)\n\n");

    while (1) {

        printf("Client: ");
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
            printf("\n[CLIENT]: You terminated the chat session.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            printf("\n[CLIENT]: Server closed the connection.\n");
            break;
        }

        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("[Server]: %s\n", buffer);

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[CLIENT]: Server ended the chat session. Exiting...\n");
            break;
        }
    }

    CLOSE_SOCKET(client_fd);
    CLEANUP_NETWORK();
    printf("Client socket closed.\n");
    return 0;
}
