#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);


    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server started...\n");
    printf("Waiting for clients...\n");

    while (1)
    {
        
        client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &addr_len);

        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

       
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
        }
        else if (pid == 0)
        {
           

            char buffer[BUFFER_SIZE];
            int bytes_read;

           
            close(server_fd);

            
            bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

            if (bytes_read > 0)
            {
                buffer[bytes_read] = '\0';

                
                char *student_id = strtok(buffer, "\n");

               
                char *file_data = strtok(NULL, "");

                char response[200];
                int found = 0;

                if (student_id != NULL && file_data != NULL)
                {
                    char data_copy[BUFFER_SIZE];
                    strcpy(data_copy, file_data);

                    char *line = strtok(data_copy, "\n");

                    while (line != NULL)
                    {
                        char id[20];

                        sscanf(line, "%s", id);

                        if (strcmp(id, student_id) == 0)
                        {
                            snprintf(response, sizeof(response),
                                     "Record Found : %s", line);

                            found = 1;
                            break;
                        }

                        line = strtok(NULL, "\n");
                    }
                }

                if (!found)
                {
                    strcpy(response, "Record Not Found");
                }

                
                write(client_fd, response, strlen(response));
            }

            close(client_fd);
            exit(0);
        }
        else
        {
           
            close(client_fd);
        }
    }

    close(server_fd);

    return 0;
}