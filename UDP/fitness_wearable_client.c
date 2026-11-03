/*
 * ============================================================================
 * MODEL QUESTION 2: FITNESS TRACKING WEARABLE (UDP SENDER / CLIENT)
 * Requirement: Lightweight, low-overhead client using UDP.
 * Sends periodic biometric telemetry without connection overhead.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #ifdef _MSC_VER
        #pragma comment(lib, "ws2_32.lib")
    #endif
    typedef int socklen_t;
    #define CLOSE_SOCKET(s) closesocket(s)
    #define SLEEP_MS(ms) Sleep(ms)
    #define INIT_NETWORK() do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while(0)
    #define CLEANUP_NETWORK() WSACleanup()
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET(s) close(s)
    #define SLEEP_MS(ms) usleep((ms) * 1000)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
#endif

#define FITNESS_PORT 7007
#define SERVER_IP "127.0.0.1"

typedef struct {
    int seq_no;
    char device_name[24];
    int heart_rate;
    int step_count;
    float calories;
} FitnessData;

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in dest_addr;
    const char *target_ip = (argc > 1) ? argv[1] : SERVER_IP;
    int total_readings = (argc > 2) ? atoi(argv[2]) : 10;

    INIT_NETWORK();

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(FITNESS_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);

    printf("======================================================================\n");
    printf("         WEARABLE FITNESS BAND TELEMETRY (UDP CLIENT)                 \n");
    printf("   Destination App: %s:%d | Total Readings: %d                        \n",
           target_ip, FITNESS_PORT, total_readings);
    printf("======================================================================\n\n");

    FitnessData data;
    strncpy(data.device_name, "FIT-PULSE-X1", sizeof(data.device_name));
    data.heart_rate = 74;
    data.step_count = 3200;
    data.calories = 145.0f;

    for (int i = 1; i <= total_readings; i++) {
        data.seq_no = i;

        data.step_count += 25 + (i * 3);
        data.calories += 1.8f;
        data.heart_rate += (i <= 5) ? 8 : -3;

        int sent_bytes = sendto(client_fd, (const char *)&data, sizeof(data), 0,
                                (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (sent_bytes < 0) {
            perror("sendto error");
            break;
        }

        printf("[TX Reading #%02d] HR: %3d BPM | Steps: %5d | Calories: %5.1f kcal (%d bytes sent)\n",
               data.seq_no, data.heart_rate, data.step_count, data.calories, sent_bytes);

        SLEEP_MS(1000);
    }

    data.seq_no = -1;
    sendto(client_fd, (const char *)&data, sizeof(data), 0,
           (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    printf("\n[WEARABLE]: Telemetry transmission finished. Band entered sleep mode.\n");

    CLOSE_SOCKET(client_fd);
    CLEANUP_NETWORK();
    return 0;
}
