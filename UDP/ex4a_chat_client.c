/*
 * ============================================================================
 * EX. NO: 4a - UDP SOCKET PROGRAMMING (CHAT CLIENT)
 * Description: Two-way interactive Chat Client using UDP Sockets in C.
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
#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    const char *target_ip = (argc > 1) ? argv[1] : SERVER_IP;

    INIT_NETWORK();

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(target_ip);

    printf("====================================================\n");
    printf("        UDP CHAT CLIENT CONNECTED TO %s:%d          \n", target_ip, PORT);
    printf("       Type message and press Enter ('exit' to quit)\n");
    printf("====================================================\n\n");

    while (1) {

        printf("Client: ");
        memset(buffer, 0, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (sendto(client_fd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&server_addr, addr_len) < 0) {
            perror("sendto error");
            break;
        }

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[Client]: You ended the chat session.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr *)&server_addr, &addr_len);
        if (bytes_received < 0) {
            perror("recvfrom error");
            break;
        }

        buffer[bytes_received] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("[Server]: %s\n", buffer);

        if (strcasecmp(buffer, "exit") == 0 || strcasecmp(buffer, "bye") == 0) {
            printf("\n[Client]: Server has left the chat. Exiting...\n");
            break;
        }
    }

    CLOSE_SOCKET(client_fd);
    CLEANUP_NETWORK();
    printf("Client socket closed.\n");
    return 0;
}
