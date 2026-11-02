#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    char buffer[1024];

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("UDP Server waiting...\n");

    while (1) {

        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1,
                         0,
                         (struct sockaddr *)&client,
                         &len);

        buffer[n] = '\0';

        printf("Received: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0)
            break;

        sendto(sockfd, "ACK",
               3, 0,
               (struct sockaddr *)&client,
               len);
    }

    close(sockfd);

    return 0;
}