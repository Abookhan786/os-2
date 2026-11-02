#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        return 1;
    }

    printf("UDP Server is running on port %d...\n", PORT);

    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &len);

        if (n < 0) {
            printf("Receive failed\n");
            continue;
        }

        buffer[n] = '\0';

        printf("Client: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        sendto(sockfd, "Message Received\n",
               strlen("Message Received\n"), 0,
               (struct sockaddr *)&client_addr, len);
    }

    close(sockfd);

    return 0;
}