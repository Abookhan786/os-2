/*
 * ============================================================================
 * DNS SERVER USING UDP SOCKETS IN C
 * ============================================================================
 * Description:
 *   - Simulates a Domain Name System (DNS) Server over UDP (SOCK_DGRAM).
 *   - Listens on port 8053 for DNS query requests from DNS clients.
 *   - Resolves:
 *       1. Forward DNS  : Domain Name -> IP Address
 *       2. Reverse DNS  : IP Address  -> Domain Name
 *   - Checks local DNS cache/table first; if not present, optionally queries
 *     the system resolver (gethostbyname) for live resolution.
 *   - Supports Windows (MinGW/MSVC) and Linux/macOS seamlessly.
 *
 * Compilation:
 *   - Windows (MinGW): gcc dns_server.c -o dns_server -lws2_32
 *   - Linux / macOS  : gcc dns_server.c -o dns_server
 *
 * Execution:
 *   ./dns_server [optional_port]
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    #define STRCASECMP(a, b) _stricmp(a, b)
#else
    #include <unistd.h>
    #include <strings.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #define CLOSE_SOCKET(s) close(s)
    #define INIT_NETWORK() do {} while(0)
    #define CLEANUP_NETWORK() do {} while(0)
    #define STRCASECMP(a, b) strcasecmp(a, b)
#endif

#define DEFAULT_PORT 8053
#define BUFFER_SIZE 1024

// Structure for DNS Database Record
typedef struct {
    char domain[128];
    char ip[64];
} DNSEntry;

// Local DNS Database / Cache
static const DNSEntry dns_table[] = {
    {"www.google.com",    "142.250.190.46"},
    {"www.yahoo.com",     "98.137.11.163"},
    {"www.github.com",    "140.82.121.4"},
    {"www.amazon.com",    "205.251.242.103"},
    {"www.microsoft.com", "20.112.52.29"},
    {"www.annauniv.edu",  "14.139.161.4"},
    {"www.mit.edu",       "18.25.32.100"},
    {"localhost",         "127.0.0.1"}
};

static const int TABLE_SIZE = sizeof(dns_table) / sizeof(dns_table[0]);

// Function to check if a string is an IP address format
int is_ip_address(const char *str) {
    int dots = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') dots++;
        else if (!isdigit((unsigned char)str[i])) return 0;
    }
    return (dots == 3);
}

// Perform DNS Lookup
void resolve_dns(const char *query, char *response, size_t resp_len) {
    // 1. Check if query is Reverse DNS (IP -> Domain)
    if (is_ip_address(query)) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (strcmp(dns_table[i].ip, query) == 0) {
                snprintf(response, resp_len, "SUCCESS|REVERSE|%s|%s", query, dns_table[i].domain);
                return;
            }
        }
        snprintf(response, resp_len, "ERROR|Domain name not found for IP '%s' in DNS table", query);
        return;
    }

    // 2. Check Local DNS Cache for Forward DNS (Domain -> IP)
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (STRCASECMP(dns_table[i].domain, query) == 0) {
            snprintf(response, resp_len, "SUCCESS|FORWARD|%s|%s", query, dns_table[i].ip);
            return;
        }
    }

    // 3. Optional Fallback to System Resolver (gethostbyname)
    struct hostent *he = gethostbyname(query);
    if (he != NULL && he->h_addr_list[0] != NULL) {
        char *resolved_ip = inet_ntoa(*(struct in_addr *)he->h_addr_list[0]);
        snprintf(response, resp_len, "SUCCESS|LIVE_RESOLVED|%s|%s", query, resolved_ip);
        return;
    }

    // 4. Domain Not Found
    snprintf(response, resp_len, "ERROR|Domain '%s' not found in DNS database", query);
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;

    INIT_NETWORK();

    // 1. Create UDP Socket (SOCK_DGRAM)
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[Server] Socket creation failed");
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    // 2. Prepare Server Address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
    server_addr.sin_port = htons(port);

    // 3. Bind UDP Socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Server] Bind failed");
        CLOSE_SOCKET(server_fd);
        CLEANUP_NETWORK();
        exit(EXIT_FAILURE);
    }

    printf("================================================================\n");
    printf("   DNS SERVER (UDP Socket Simulation)                          \n");
    printf("   Listening on Port : %d                                      \n", port);
    printf("   Protocol          : UDP (SOCK_DGRAM)                        \n");
    printf("================================================================\n");
    printf("Local DNS Table Entries:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("  %-22s -> %s\n", dns_table[i].domain, dns_table[i].ip);
    }
    printf("----------------------------------------------------------------\n");
    printf("[Server] Waiting for client DNS queries...\n\n");

    // 4. Server Request-Response Loop
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));

        // Receive DNS query from client
        int bytes_received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("[Server] recvfrom error");
            continue;
        }

        buffer[bytes_received] = '\0';

        // Trim newline and carriage returns
        buffer[strcspn(buffer, "\r\n")] = '\0';

        char *client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = ntohs(client_addr.sin_port);

        printf("[Query Received] From [%s:%d] : \"%s\"\n", client_ip, client_port, buffer);

        // Check for exit message
        if (STRCASECMP(buffer, "exit") == 0 || STRCASECMP(buffer, "quit") == 0) {
            printf("[Server] Client [%s:%d] disconnected.\n\n", client_ip, client_port);
            continue;
        }

        // Perform DNS Resolution
        resolve_dns(buffer, response, sizeof(response));

        printf("[Server Response] Sending to [%s:%d] : %s\n\n", client_ip, client_port, response);

        // Send resolved answer back to client
        sendto(server_fd, response, (int)strlen(response), 0,
               (struct sockaddr *)&client_addr, client_len);
    }

    CLOSE_SOCKET(server_fd);
    CLEANUP_NETWORK();
    return 0;
}
