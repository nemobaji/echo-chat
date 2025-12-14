#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define SERVER_IP "127.0.0.1"

void error_handling(char *message);

int main(int argc, char *argv[]) {
    int client_sock;
    char message[BUFFER_SIZE];
    int str_len;

    struct sockaddr_in server_addr;

    client_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) 
        error_handling("Error: socket() error");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (connect(client_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
        error_handling("Error: connect() error");
    printf("> Connected to echo server (%s:%d).\n", SERVER_IP, PORT);
    
    while(1) {
        printf("> Input messsage (Q/q to quit): ");
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) break;
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;

        write(client_sock, message, strlen(message));
        str_len = read(client_sock, message, BUFFER_SIZE - 1);
        if (str_len == -1)
            error_handling("Error: read() error");
        
        message[str_len] = 0;
        printf("> Echo from server: %s", message);
    }

    close(client_sock);
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}