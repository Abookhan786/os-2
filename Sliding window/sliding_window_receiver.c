#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_DATA 500
#define TOTAL_PACKETS 6

typedef struct {
    int seq;
    char data[MAX_DATA];
} Packet;

typedef struct {
    int seq;
} Ack;

int send_all(int sock, void *data, int size)
{
    int sent = 0;

    while (sent < size) {

        int n = send(sock,
                     (char *)data + sent,
                     size - sent,
                     0);

        if (n <= 0)
            return -1;

        sent += n;
    }

    return 0;
}

int receive_all(int sock, void *data, int size)
{
    int received = 0;

    while (received < size) {

        int n = recv(sock,
                     (char *)data + received,
                     size - received,
                     0);

        if (n <= 0)
            return -1;

        received += n;
    }

    return 0;
}

int main()
{
    int server_fd;
    int client_fd;

    struct sockaddr_in server;
    struct sockaddr_in client;

    socklen_t client_len = sizeof(client);

    Packet packet;
    Ack ack;

    int received[TOTAL_PACKETS] = {0};
    int ack_lost[TOTAL_PACKETS] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {

        perror("Socket creation failed");
        return 1;
    }

    int option = 1;

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &option,
               sizeof(option));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd,
             (struct sockaddr *)&server,
             sizeof(server)) < 0) {

        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {

        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("\n========================================\n");
    printf("    SLIDING WINDOW RECEIVER\n");
    printf("========================================\n");

    printf("\n[Receiver] Waiting for sender...\n");

    client_fd = accept(server_fd,
                       (struct sockaddr *)&client,
                       &client_len);

    if (client_fd < 0) {

        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    printf("[Receiver] Sender connected.\n\n");

    while (1) {

        memset(&packet, 0, sizeof(Packet));

        if (receive_all(client_fd,
                        &packet,
                        sizeof(Packet)) < 0) {

            break;
        }

        int index = packet.seq - 1;

        printf("----------------------------------------\n");

        if (!received[index]) {

            received[index] = 1;

            printf("[Receiver] Packet %d received.\n",
                   packet.seq);

            printf("[Receiver] Message: %s\n",
                   packet.data);

            if (packet.seq == 4 &&
                ack_lost[index] == 0) {

                ack_lost[index] = 1;

                printf("[Receiver] ACK %d LOST!\n",
                       packet.seq);

                printf("[Receiver] ACK was not sent.\n");

                continue;
            }

            ack.seq = packet.seq;

            printf("[Receiver] Sending ACK %d\n",
                   ack.seq);

            if (send_all(client_fd,
                         &ack,
                         sizeof(Ack)) < 0) {

                break;
            }
        }

        else {

            printf("[Receiver] Duplicate Packet %d received.\n",
                   packet.seq);

            printf("[Receiver] Packet already processed.\n");

            ack.seq = packet.seq;

            printf("[Receiver] Sending ACK %d again.\n",
                   ack.seq);

            if (send_all(client_fd,
                         &ack,
                         sizeof(Ack)) < 0) {

                break;
            }
        }

        int count = 0;

        for (int i = 0; i < TOTAL_PACKETS; i++) {

            if (received[i])
                count++;
        }

        if (count == TOTAL_PACKETS) {

            printf("\n========================================\n");
            printf("[Receiver] All packets received.\n");
            printf("========================================\n");

            continue;
        }
    }

    close(client_fd);
    close(server_fd);

    return 0;
}