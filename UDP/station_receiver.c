/*
 * ============================================================================
 * PROBLEM 1: DRONE TELEMETRY TRACKING SYSTEM - MONITORING STATION (RECEIVER)
 * Requirement: Fast, lightweight real-time telemetry receiver using UDP.
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

#define STATION_PORT 5005
#define BUFFER_SIZE 512

typedef struct {
    int seq_num;
    char drone_id[16];
    double latitude;
    double longitude;
    float altitude;
    float speed;
    int battery;
} DroneTelemetry;

int main() {
    int station_fd;
    struct sockaddr_in station_addr, drone_addr;
    socklen_t addr_len = sizeof(drone_addr);
    DroneTelemetry telemetry;

    INIT_NETWORK();

    if ((station_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Station socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&station_addr, 0, sizeof(station_addr));
    station_addr.sin_family = AF_INET;
    station_addr.sin_addr.s_addr = INADDR_ANY;
    station_addr.sin_port = htons(STATION_PORT);

    if (bind(station_fd, (struct sockaddr *)&station_addr, sizeof(station_addr)) < 0) {
        perror("Station bind failed");
        CLOSE_SOCKET(station_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("========================================================================================\n");
    printf("                DRONE GROUND MONITORING STATION (UDP RECEIVER)                          \n");
    printf("                Listening on Port: %d | Protocol: UDP (Lightweight)                    \n", STATION_PORT);
    printf("========================================================================================\n");
    printf("%-5s | %-12s | %-11s | %-11s | %-12s | %-11s | %-7s | %-10s\n",
           "Seq#", "Drone ID", "Latitude", "Longitude", "Altitude(m)", "Speed(km/h)", "Batt %", "Status");
    printf("------+--------------+-------------+-------------+--------------+-------------+---------+-----------\n");

    while (1) {
        int bytes = recvfrom(station_fd, (char *)&telemetry, sizeof(telemetry), 0,
                             (struct sockaddr *)&drone_addr, &addr_len);
        if (bytes < 0) {
            perror("recvfrom error");
            break;
        }

        if (telemetry.seq_num == -1) {
            printf("------+--------------+-------------+-------------+--------------+-------------+---------+-----------\n");
            printf("[STATION]: Drone '%s' landed / finished transmission.\n", telemetry.drone_id);
            break;
        }

        char status[16] = "OPTIMAL";
        if (telemetry.battery < 20) {
            strcpy(status, "LOW BATT");
        } else if (telemetry.altitude > 200.0f) {
            strcpy(status, "HIGH ALT");
        } else if (telemetry.speed > 80.0f) {
            strcpy(status, "HIGH SPD");
        }

        printf("%-5d | %-12s | %11.6f | %11.6f | %12.2f | %11.2f | %6d%% | %-10s\n",
               telemetry.seq_num,
               telemetry.drone_id,
               telemetry.latitude,
               telemetry.longitude,
               telemetry.altitude,
               telemetry.speed,
               telemetry.battery,
               status);
        fflush(stdout);
    }

    printf("========================================================================================\n");
    printf("Monitoring session closed.\n");

    CLOSE_SOCKET(station_fd);
    CLEANUP_NETWORK();
    return 0;
}
