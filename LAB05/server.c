#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h>

# define PORT 8080
# define BUFFER_SIZE 1024

#include <stdio.h>

int count_words(char *str) {     
    int count = 0, in_word = 0;      
    
    while (*str != '\0') {         
        char ch = *str;         
        if (ch != ' ' && ch != '\t' && ch != '\n') {        
        if (in_word == 0) {             
            in_word = 1;             
            count++;         
        } 
        else{
            in_word = 0;
        }  
    }      
          str++;    
    }     
    return count; 
}

int count_vowels (char * str ) {
int count = 0;
while(*str!='\0'){
    char ch = *str;
    if(ch>='A'&& ch<='Z'){
        ch = ch + 32;
    }
    if(ch=='a' || ch=='e' || ch=='i' || ch=='o' || ch=='u')
        count++;
         str++;
}     

return count ;
}

int main () {
int server_fd , new_socket ;
struct sockaddr_in address ;
int addrlen = sizeof(address);
char buffer [ BUFFER_SIZE ];
server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
       return 1;
    }
new_socket = accept(server_fd,(struct sockaddr *)&address,(socklen_t *)&addrlen);

    if (new_socket < 0)
    {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }

    printf("Client connected.\n");
while (1) {
   memset ( buffer , 0 , BUFFER_SIZE ) ;
    int valread = read ( new_socket , buffer ,BUFFER_SIZE );
    printf("%s\n", buffer);

    if ( valread <= 0) 
    break ;
    buffer[strcspn ( buffer , "\n" ) ] = 0;

    if ( strcmp ( buffer , " bye " ) == 0) 
    break ;
     char command [20] , text [ BUFFER_SIZE ];
    sscanf ( buffer ,  "%[^|]|%[^\n ]" , command , text ) ;
    if (strcmp(command, "ANALYZE") == 0) {

        int chars = strlen ( text ) ;
        int words = count_words ( text ) ;
        int vowels = count_vowels ( text ) ;
        
    char response [ BUFFER_SIZE ];
    sprintf ( response , " Chars =% d , ␣ Words =% d , ␣Vowels =% d " , chars , words , vowels ) ;
    printf("%s\n", response);
    send ( new_socket , response , strlen ( response ) ,0) ;
} 
else 
{
char * msg = " Invalid ␣ Command " ;

send ( new_socket , msg , strlen ( msg ) , 0) ;
}
}
    printf("Client disconnected.\n");
    close(new_socket);
    close(server_fd);

    return 0;
}