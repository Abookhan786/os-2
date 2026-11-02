/*
 * ============================================================================
 * PROBLEM 2: FACTORY MONITORING SYSTEM - MACHINE SENSOR UNIT (SENDER)
 * Requirement: Fast, lightweight industrial machine telemetry sender using UDP.
 * Transmits sensor readings with zero connection overhead.
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

#define CONTROL_ROOM_PORT 6006
#define CONTROL_ROOM_IP "127.0.0.1"

/* Factory Machine Status Telemetry Structure */
typedef struct {
    int seq_no;
    int machine_id;
    char machine_name[32];
    float temperature;    // in Celsius
    float pressure;       // in Bar
    float vibration;      // in mm/s
    int rpm;              // Motor RPM
} MachineStatus;

int main(int argc, char *argv[]) {
    int sender_fd;
    struct sockaddr_in dest_addr;
    const char *target_ip = (argc > 1) ? argv[1] : CONTROL_ROOM_IP;
    int total_reports = (argc > 2) ? atoi(argv[2]) : 10;

    INIT_NETWORK();

    // 1. Create UDP socket
    if ((sender_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    // 2. Set Control Room Address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(CONTROL_ROOM_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);

    printf("======================================================================\n");
    printf("        FACTORY SENSOR TELEMETRY UNIT (UDP TRANSMITTER)               \n");
    printf("  Target Control Room: %s:%d | Reports to send: %d                   \n",
           target_ip, CONTROL_ROOM_PORT, total_reports);
    printf("======================================================================\n\n");

    MachineStatus machine;
    machine.machine_id = 402;
    strncpy(machine.machine_name, "CNC-Hydraulic-Press", sizeof(machine.machine_name));
    machine.temperature = 62.0f;
    machine.pressure = 4.2f;
    machine.vibration = 1.4f;
    machine.rpm = 2400;

    for (int i = 1; i <= total_reports; i++) {
        machine.seq_no = i;

        // Simulate sensor variations under operating load
        machine.temperature += 2.5f;       // Temperature increases as work continues
        machine.pressure += (i % 2 == 0) ? 0.35f : -0.15f;
        machine.vibration += 0.38f;        // Vibration gradually rises
        machine.rpm += (i % 2 == 0) ? 50 : -30;

        // 3. Fast datagram transmission (no handshake or connection latency)
        int sent_bytes = sendto(sender_fd, (const char *)&machine, sizeof(machine), 0,
                                (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (sent_bytes < 0) {
            perror("sendto failed");
            break;
        }

        printf("[TX #%02d] Machine #%d (%s) -> Temp: %.1f C, Press: %.2f Bar, Vib: %.2f mm/s, RPM: %d (%d bytes)\n",
               machine.seq_no, machine.machine_id, machine.machine_name,
               machine.temperature, machine.pressure, machine.vibration, machine.rpm, sent_bytes);

        // Transmit every 1 second (1000 ms) for real-time factory monitoring
        SLEEP_MS(1000);
    }

    // Send end indicator
    machine.seq_no = -1;
    sendto(sender_fd, (const char *)&machine, sizeof(machine), 0,
           (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    printf("\n[FACTORY UNIT]: Monitoring transmission finished.\n");

    CLOSE_SOCKET(sender_fd);
    CLEANUP_NETWORK();
    return 0;
}
