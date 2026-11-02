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
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock,
                (struct sockaddr *)&server,
                sizeof(server)) < 0) {

        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("[Client] Connected to server.\n");

    // Send message
    char message[] = "Hello from client";

    send(sock,
         message,
         strlen(message) + 1,
         0);

    printf("[Client] Message sent.\n");

    // Receive response
    memset(buffer, 0, SIZE);

    recv(sock, buffer, SIZE, 0);

    printf("[Client] Received: %s\n", buffer);

    // Close socket
    close(sock);

    return 0;
}