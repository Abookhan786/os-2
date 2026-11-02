#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 8080
#define MAX_DATA 500
#define TOTAL_PACKETS 6
#define WINDOW_SIZE 3
#define TIMEOUT 3

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
        int n = send(sock, (char *)data + sent, size - sent, 0);

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
        int n = recv(sock, (char *)data + received,
                     size - received, 0);

        if (n <= 0)
            return -1;

        received += n;
    }

    return 0;
}

int main()
{
    int sock;
    struct sockaddr_in server;

    Packet packets[TOTAL_PACKETS];
    Ack ack;

    int acknowledged[TOTAL_PACKETS] = {0};
    int attempts[TOTAL_PACKETS] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server,
                sizeof(server)) < 0) {

        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("\n========================================\n");
    printf("    SLIDING WINDOW PROTOCOL\n");
    printf("========================================\n");

    printf("\n[Sender] Connected to receiver.\n");

    printf("\nEnter %d messages:\n\n", TOTAL_PACKETS);

    for (int i = 0; i < TOTAL_PACKETS; i++) {

        packets[i].seq = i + 1;

        printf("Message for Packet %d: ", i + 1);
        scanf(" %[^\n]", packets[i].data);
    }

    int base = 0;
    int next = 0;

    while (base < TOTAL_PACKETS) {

        printf("\n----------------------------------------\n");
        printf("Current Window: ");

        for (int i = base;
             i < base + WINDOW_SIZE && i < TOTAL_PACKETS;
             i++) {

            if (!acknowledged[i])
                printf("[%d] ", i + 1);
        }

        printf("\n----------------------------------------\n");

        while (next < base + WINDOW_SIZE &&
               next < TOTAL_PACKETS) {

            if (!acknowledged[next]) {

                attempts[next]++;

                if (next == 1 && attempts[next] == 1) {

                    printf("\n[Sender] Packet %d LOST during transmission!\n",
                           next + 1);

                    printf("[Sender] Packet %d was not received by receiver.\n",
                           next + 1);
                }
                else {

                    printf("\n[Sender] Sending Packet %d: %s\n",
                           packets[next].seq,
                           packets[next].data);

                    if (send_all(sock,
                                 &packets[next],
                                 sizeof(Packet)) < 0) {

                        perror("Send failed");
                        close(sock);
                        return 1;
                    }
                }
            }

            next++;
        }

        while (base < next) {

            if (acknowledged[base]) {
                base++;
                continue;
            }

            fd_set readfds;

            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);

            struct timeval timeout;

            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            int result = select(sock + 1,
                                &readfds,
                                NULL,
                                NULL,
                                &timeout);

            if (result == 0) {

                printf("\n[Sender] TIMEOUT!\n");

                printf("[Sender] ACK for Packet %d not received.\n",
                       base + 1);

                printf("[Sender] Retransmitting Packet %d...\n",
                       base + 1);

                attempts[base]++;

                printf("[Sender] Sending Packet %d again: %s\n",
                       packets[base].seq,
                       packets[base].data);

                if (send_all(sock,
                             &packets[base],
                             sizeof(Packet)) < 0) {

                    perror("Retransmission failed");
                    close(sock);
                    return 1;
                }

                continue;
            }

            if (result < 0) {

                perror("select failed");
                close(sock);
                return 1;
            }

            if (FD_ISSET(sock, &readfds)) {

                if (receive_all(sock, &ack, sizeof(Ack)) < 0) {

                    printf("[Sender] Receiver disconnected.\n");
                    close(sock);
                    return 1;
                }

                int index = ack.seq - 1;

                if (index >= 0 &&
                    index < TOTAL_PACKETS &&
                    !acknowledged[index]) {

                    acknowledged[index] = 1;

                    printf("[Sender] ACK %d received.\n",
                           ack.seq);

                    while (base < TOTAL_PACKETS &&
                           acknowledged[base]) {

                        base++;
                    }

                    break;
                }
            }
        }
    }

    printf("\n========================================\n");
    printf("[Sender] All packets transmitted successfully.\n");
    printf("[Sender] All acknowledgements received.\n");
    printf("========================================\n");

    close(sock);

    return 0;
}