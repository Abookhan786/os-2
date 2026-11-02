#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    char buffer[1024];
    char message[1024];

    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    for (int i = 1; i <= 5; i++) {

        sprintf(message, "DATA %d", i);

        printf("Sending: %s\n", message);

        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&server, len);

        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1,
                         0, NULL, NULL);

        buffer[n] = '\0';

        printf("Received: %s\n\n", buffer);
    }

    close(sockfd);

    return 0;
}