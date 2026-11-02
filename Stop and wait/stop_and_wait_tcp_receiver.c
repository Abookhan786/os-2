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

    char frame[SIZE];
    char ack[SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, 5);

    printf("[Receiver] Waiting for sender...\n");

    client_fd = accept(
        server_fd,
        (struct sockaddr *)&client,
        &client_len
    );

    if (client_fd < 0) {
        perror("Accept failed");
        return 1;
    }

    printf("[Receiver] Sender connected.\n\n");

    for (int i = 0; i < 5; i++) {

        memset(frame, 0, SIZE);

        recv(client_fd, frame, SIZE, 0);

        printf("[Receiver] Received %s\n", frame);

        sprintf(ack, "ACK %d", i);

        printf("[Receiver] Sending %s\n\n", ack);

        send(client_fd, ack, strlen(ack) + 1, 0);
    }

    printf("[Receiver] All frames received successfully.\n");

    close(client_fd);
    close(server_fd);

    return 0;
}