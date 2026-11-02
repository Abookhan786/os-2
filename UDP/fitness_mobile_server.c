/*
 * ============================================================================
 * MODEL QUESTION 2: FITNESS TRACKER MOBILE APP (UDP RECEIVER / SERVER)
 * Requirement: Lightweight, low-overhead real-time receiver using UDP.
 * Prioritizes speed and low latency over guaranteed delivery.
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
    #define INIT_NETWORK() do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while(0)
    #define CLEANUP_NETWORK() WSACleanup()
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET(s) close(s)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
#endif

#define FITNESS_PORT 7007

/* Lightweight Fitness Telemetry Frame */
typedef struct {
    int seq_no;
    char device_name[24];
    int heart_rate;   // Beats per minute (BPM)
    int step_count;   // Total steps taken
    float calories;   // Active kcal burned
} FitnessData;

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    FitnessData data;

    INIT_NETWORK();

    // 1. Create UDP Socket (SOCK_DGRAM - zero connection overhead)
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    // 2. Bind to Mobile App Reception Port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(FITNESS_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("======================================================================================\n");
    printf("              MOBILE APP FITNESS TRACKER DASHBOARD (UDP SERVER)                       \n");
    printf("              Listening on Port: %d | Low-Overhead Real-Time Stream                   \n", FITNESS_PORT);
    printf("======================================================================================\n");
    printf("%-6s | %-16s | %-11s | %-12s | %-11s | %-18s\n",
           "Seq#", "Device Model", "Heart (BPM)", "Step Count", "Calories", "Activity Zone");
    printf("-------+------------------+-------------+--------------+-------------+------------------------\n");

    while (1) {
        // 3. Receive lightweight datagram
        int bytes_received = recvfrom(server_fd, (char *)&data, sizeof(data), 0,
                                      (struct sockaddr *)&client_addr, &addr_len);

        if (bytes_received < 0) {
            perror("recvfrom error");
            break;
        }

        // Check for session end signal
        if (data.seq_no == -1) {
            printf("-------+------------------+-------------+--------------+-------------+------------------------\n");
            printf("[MOBILE APP]: Sync session ended by wearable device '%s'.\n", data.device_name);
            break;
        }

        // Evaluate Heart Rate Zone
        char zone[32] = "[OPTIMAL / REST]";
        if (data.heart_rate >= 135) {
            strcpy(zone, "! HIGH HR / PEAK !");
        } else if (data.heart_rate >= 110) {
            strcpy(zone, "* CARDIO / AEROBIC *");
        } else if (data.heart_rate >= 85) {
            strcpy(zone, "FAT BURN ZONE");
        }

        printf("#%-5d | %-16s | %4d BPM    | %6d steps | %6.1f kcal | %-18s\n",
               data.seq_no,
               data.device_name,
               data.heart_rate,
               data.step_count,
               data.calories,
               zone);
        fflush(stdout);
    }

    printf("======================================================================================\n");
    printf("Fitness receiver session finished.\n");

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
