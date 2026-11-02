#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main() {
int server_fd, client_socket;
struct sockaddr_in server_addr, client_addr;
socklen_t addr_len = sizeof(client_addr);
char buffer[BUFFER_SIZE];
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd < 0) {
perror("Socket");
exit(EXIT_FAILURE);
}
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);
if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
perror("Bind");
exit(EXIT_FAILURE);
}
listen(server_fd, 5);
printf("Server is waiting for connection...\n");
client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
if (client_socket < 0) {
perror("Accept");
exit(EXIT_FAILURE);
}
printf("Client connected.\n");
while (1) {
memset(buffer, 0, BUFFER_SIZE);
int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);
if (bytes <= 0) {
printf("Client disconnected.\n");
break;
}
printf("Client: %s\n", buffer);
if (strcmp(buffer, "exit") == 0) {
printf("Client disconnected.\n");
break;
}
}
close(client_socket);
close(server_fd);
return 0;
}