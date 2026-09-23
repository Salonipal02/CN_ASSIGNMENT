#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    char student_id[50];
    char file_data[BUFFER_SIZE];
    char send_data[BUFFER_SIZE];
    char response[200];
  
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(1);
    }


    FILE *file = fopen("data.txt", "r");

    if (file == NULL)
    {
        perror("Unable to open data.txt");
        close(sock);
        exit(1);
    }


    size_t n = fread(file_data, 1, sizeof(file_data) - 1, file);

    file_data[n] = '\0';

    fclose(file);
  
    printf("Enter Student ID to search : ");
    scanf("%s", student_id);

    
    snprintf(send_data, sizeof(send_data),
             "%s\n%s", student_id, file_data);

    
    write(sock, send_data, strlen(send_data));
    
    int bytes_received = read(sock, response,
                              sizeof(response) - 1);

    if (bytes_received > 0)
    {
        response[bytes_received] = '\0';

        printf("Server Response : %s\n", response);
    }
    close(sock);

    return 0;
}