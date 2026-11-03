/*
 * ============================================================================
 * PROBLEM 2: FACTORY MONITORING SYSTEM - CONTROL ROOM (RECEIVER)
 * Requirement: Fast, lightweight industrial machine telemetry receiver using UDP.
 * Prioritizes speed and real-time state over retransmission delays.
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

#define CONTROL_ROOM_PORT 6006

typedef struct {
    int seq_no;
    int machine_id;
    char machine_name[32];
    float temperature;
    float pressure;
    float vibration;
    int rpm;
} MachineStatus;

int main() {
    int server_fd;
    struct sockaddr_in server_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    MachineStatus status_pkt;

    INIT_NETWORK();

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(CONTROL_ROOM_PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("================================================================================================\n");
    printf("                  CENTRAL FACTORY CONTROL ROOM - REAL-TIME MONITORING                          \n");
    printf("                    Listening on UDP Port: %d | Low-Latency Telemetry                          \n", CONTROL_ROOM_PORT);
    printf("================================================================================================\n");
    printf("%-5s | %-6s | %-20s | %-9s | %-10s | %-11s | %-7s | %-18s\n",
           "Seq#", "M-ID", "Machine Name", "Temp(C)", "Press(Bar)", "Vib(mm/s)", "RPM", "Safety State");
    printf("------+--------+----------------------+-----------+------------+-------------+---------+------------------\n");

    while (1) {
        int bytes = recvfrom(server_fd, (char *)&status_pkt, sizeof(status_pkt), 0,
                             (struct sockaddr *)&sender_addr, &addr_len);
        if (bytes < 0) {
            perror("recvfrom error");
            break;
        }

        if (status_pkt.seq_no == -1) {
            printf("------+--------+----------------------+-----------+------------+-------------+---------+------------------\n");
            printf("[CONTROL ROOM]: Sensor telemetry stream ended by Machine #%d.\n", status_pkt.machine_id);
            break;
        }

        char state_desc[32] = "[NORMAL]";
        if (status_pkt.temperature >= 85.0f || status_pkt.vibration >= 5.0f) {
            strcpy(state_desc, "! DANGER CRITICAL !");
        } else if (status_pkt.temperature >= 75.0f || status_pkt.vibration >= 3.8f || status_pkt.pressure >= 6.0f) {
            strcpy(state_desc, "* WARNING HIGH *");
        }

        printf("%-5d | #%-5d | %-20s | %7.1f C | %7.2f Bar | %8.2f mm/s | %6d | %-18s\n",
               status_pkt.seq_no,
               status_pkt.machine_id,
               status_pkt.machine_name,
               status_pkt.temperature,
               status_pkt.pressure,
               status_pkt.vibration,
               status_pkt.rpm,
               state_desc);
        fflush(stdout);
    }

    printf("================================================================================================\n");
    printf("Factory Control Room monitoring closed.\n");

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
