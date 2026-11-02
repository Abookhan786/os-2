#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;

    char buffer[SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send message
    char message[] = "Hello from UDP client";

    sendto(sock,
           message,
           strlen(message) + 1,
           0,
           (struct sockaddr *)&server,
           sizeof(server));

    printf("[Client] Message sent.\n");

    // Receive response
    socklen_t server_len = sizeof(server);

    memset(buffer, 0, SIZE);

    recvfrom(sock,
             buffer,
             SIZE,
             0,
             (struct sockaddr *)&server,
             &server_len);

    printf("[Client] Received: %s\n", buffer);

    // Close socket
    close(sock);

    return 0;
}