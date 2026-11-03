/*
 * ============================================================================
 * PROBLEM 1: DRONE TELEMETRY TRACKING SYSTEM - DRONE UNIT (SENDER)
 * Requirement: Fast, lightweight real-time telemetry sender using UDP.
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

#define STATION_PORT 5005
#define STATION_IP "127.0.0.1"

typedef struct {
    int seq_num;
    char drone_id[16];
    double latitude;
    double longitude;
    float altitude;
    float speed;
    int battery;
} DroneTelemetry;

int main(int argc, char *argv[]) {
    int drone_fd;
    struct sockaddr_in station_addr;
    const char *target_ip = (argc > 1) ? argv[1] : STATION_IP;
    int total_packets = (argc > 2) ? atoi(argv[2]) : 10;

    INIT_NETWORK();

    if ((drone_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Drone socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&station_addr, 0, sizeof(station_addr));
    station_addr.sin_family = AF_INET;
    station_addr.sin_port = htons(STATION_PORT);
    station_addr.sin_addr.s_addr = inet_addr(target_ip);

    printf("======================================================================\n");
    printf("         DRONE ONBOARD TELEMETRY TRANSMITTER (UDP CLIENT)             \n");
    printf("  Target Station: %s:%d | Telemetry Stream: %d packets               \n",
           target_ip, STATION_PORT, total_packets);
    printf("======================================================================\n\n");

    DroneTelemetry telemetry;
    strcpy(telemetry.drone_id, "DRONE-ALPHA1");
    telemetry.latitude = 13.082680;
    telemetry.longitude = 80.270718;
    telemetry.altitude = 45.0f;
    telemetry.speed = 32.5f;
    telemetry.battery = 100;

    for (int i = 1; i <= total_packets; i++) {
        telemetry.seq_num = i;

        telemetry.latitude += 0.00045;
        telemetry.longitude += 0.00032;
        telemetry.altitude += (i % 2 == 0) ? 5.5f : -2.0f;
        telemetry.speed += (i % 3 == 0) ? 3.0f : -1.5f;
        telemetry.battery -= 1;

        int sent_bytes = sendto(drone_fd, (const char *)&telemetry, sizeof(telemetry), 0,
                                (struct sockaddr *)&station_addr, sizeof(station_addr));

        if (sent_bytes < 0) {
            perror("Failed to send telemetry packet");
            break;
        }

        printf("[TX #%02d] Lat: %.6f, Lon: %.6f, Alt: %.1fm, Spd: %.1fkm/h, Batt: %d%% (%d bytes sent)\n",
               telemetry.seq_num, telemetry.latitude, telemetry.longitude,
               telemetry.altitude, telemetry.speed, telemetry.battery, sent_bytes);

        SLEEP_MS(1000);
    }

    telemetry.seq_num = -1;
    sendto(drone_fd, (const char *)&telemetry, sizeof(telemetry), 0,
           (struct sockaddr *)&station_addr, sizeof(station_addr));

    printf("\n[DRONE]: Telemetry stream completed. Drone landed.\n");

    CLOSE_SOCKET(drone_fd);
    CLEANUP_NETWORK();
    return 0;
}
