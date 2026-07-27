#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to server.\n");

    // Communication loop
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        // Exit condition
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Send message to server
        send(sock, buffer, strlen(buffer), 0);

        // Receive response
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("Server: %s\n", buffer);
    }

    // Close socket
    close(sock);
    printf("Connection closed.\n");

    return 0;
}