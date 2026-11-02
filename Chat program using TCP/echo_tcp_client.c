
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main() {
int sock;
struct sockaddr_in server_addr;
char buffer[BUFFER_SIZE];
sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
perror("Socket");
exit(EXIT_FAILURE);
}
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
perror("Connection");
exit(EXIT_FAILURE);
}
printf("Connected to server.\n");
while (1) {
printf("Enter message: ");
fgets(buffer, BUFFER_SIZE, stdin);
buffer[strcspn(buffer, "\n")] = '\0';
send(sock, buffer, strlen(buffer) + 1, 0);
if (strcmp(buffer, "exit") == 0)
break;
}
close(sock);
return 0;
}