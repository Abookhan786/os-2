#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    char buffer[SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_fd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0) {

        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("[Server] Waiting for client...\n");

    // Accept client
    client_fd = accept(server_fd,
                       (struct sockaddr *)&client,
                       &client_len);

    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    printf("[Server] Client connected.\n");

    // Receive message
    memset(buffer, 0, SIZE);

    recv(client_fd, buffer, SIZE, 0);

    printf("[Server] Received: %s\n", buffer);

    // Send response
    char response[] = "Hello from server";

    send(client_fd,
         response,
         strlen(response) + 1,
         0);

    printf("[Server] Response sent.\n");

    // Close sockets
    close(client_fd);
    close(server_fd);

    return 0;
}