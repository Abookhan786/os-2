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
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

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
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sock,
             (struct sockaddr *)&server,
             sizeof(server)) < 0) {

        perror("Bind failed");
        close(sock);
        return 1;
    }

    printf("[Server] Waiting for client...\n");

    // Receive message
    memset(buffer, 0, SIZE);

    recvfrom(sock,
             buffer,
             SIZE,
             0,
             (struct sockaddr *)&client,
             &client_len);

    printf("[Server] Received: %s\n", buffer);

    // Send response
    char response[] = "Hello from UDP server";

    sendto(sock,
           response,
           strlen(response) + 1,
           0,
           (struct sockaddr *)&client,
           client_len);

    printf("[Server] Response sent.\n");

    // Close socket
    close(sock);

    return 0;
}