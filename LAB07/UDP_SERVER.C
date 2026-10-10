#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30009
#define ROLL "24105142009"
#define SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len;
    char buffer[SIZE], response[SIZE];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

  
    if (bind(sockfd, (struct sockaddr *)&server,
             sizeof(server)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Server running on port %d\n", PORT);

    
    while (1) {
        len = sizeof(client);

        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&client, &len);

        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));

        printf("\nClient address: %s:%d\n",
               ip, ntohs(client.sin_port));

       
        char *sep = strchr(buffer, '#');

        if (sep == NULL) {
            strcpy(response, "ERROR: Unauthorized client");
        } else {
            *sep = '\0';

            if (strcmp(buffer, ROLL) != 0) {
                strcpy(response, "ERROR: Unauthorized client");
            } else {
                char *msg = sep + 1;

                printf("Received message: %s\n", msg);

                
                if (strcmp(msg, "EXIT") == 0) {
                    strcpy(response, "Server shutting down");

                    sendto(sockfd, response, strlen(response), 0,
                           (struct sockaddr *)&client, len);

                    printf("Sent response: %s\n", response);
                    break;
                }

               
                char *text = strchr(msg, '|');

                if (text == NULL) {
                    strcpy(response, "ERROR: Invalid request");
                } else {
                    *text = '\0';
                    text++;

                    if (strcmp(msg, "UP") == 0) {
                        int i;

                        for (i = 0; text[i] != '\0'; i++) {
                            response[i] =
                                toupper((unsigned char)text[i]);
                        }

                        response[i] = '\0';

                    } else if (strcmp(msg, "LOW") == 0) {
                        int i;

                        for (i = 0; text[i] != '\0'; i++) {
                            response[i] =
                                tolower((unsigned char)text[i]);
                        }

                        response[i] = '\0';

                    } else if (strcmp(msg, "REV") == 0) {
                        int l = strlen(text);
                        int i;

                        for (i = 0; i < l; i++) {
                            response[i] = text[l - 1 - i];
                        }

                        response[l] = '\0';

                    } else {
                        strcpy(response, "ERROR: Invalid command");
                    }
                }
            }
        }

      
        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&client, len);

        printf("Sent response: %s\n", response);
    }

    close(sockfd);
    return 0;
}