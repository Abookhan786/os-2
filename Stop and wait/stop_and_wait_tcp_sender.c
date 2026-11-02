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
    char frame[SIZE];
    char ack[SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("[Sender] Connected to receiver.\n");

    for (int i = 0; i < 5; i++) {

        sprintf(frame, "Frame %d", i);

        printf("[Sender] Sending %s\n", frame);
        send(sock, frame, strlen(frame) + 1, 0);

        memset(ack, 0, SIZE);
        recv(sock, ack, SIZE, 0);

        printf("[Sender] Received %s\n\n", ack);
    }

    printf("[Sender] All frames sent successfully.\n");

    close(sock);

    return 0;
}