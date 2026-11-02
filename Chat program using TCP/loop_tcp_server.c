#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        printf("Listen failed\n");
        return 1;
    }

    printf("Server is running on port %d...\n", PORT);

    client_socket = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &client_len);

    if (client_socket < 0) {
        printf("Accept failed\n");
        return 1;
    }

    printf("Client connected.\n");

    while (1) {
        int n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (n <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[n] = '\0';

        printf("Client: %s", buffer);

        send(client_socket, "Message Received\n",
             strlen("Message Received\n"), 0);
    }

    close(client_socket);
    close(server_fd);

    return 0;
}