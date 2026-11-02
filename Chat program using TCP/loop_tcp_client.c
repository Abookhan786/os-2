#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Connected to server.\n");

    while (1) {
        printf("Enter message (type exit to quit): ");

        fgets(message, BUFFER_SIZE, stdin);

        send(sock, message, strlen(message), 0);

        if (strncmp(message, "exit", 4) == 0)
            break;

        int n = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        buffer[n] = '\0';

        printf("Server: %s", buffer);
    }

    close(sock);

    return 0;
}