#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1",
              &server_addr.sin_addr);

    while (1) {
        printf("Enter message (type exit to quit): ");

        fgets(message, BUFFER_SIZE, stdin);

        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&server_addr, len);

        if (strncmp(message, "exit", 4) == 0)
            break;

        int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                         NULL, NULL);

        if (n < 0) {
            printf("Receive failed\n");
            continue;
        }

        buffer[n] = '\0';

        printf("Server: %s", buffer);
    }

    close(sockfd);

    return 0;
}