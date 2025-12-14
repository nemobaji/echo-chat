#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void error_handling(char *message);
void *handle_client(void *arg);

int main(int argc, char *argv[]) {
    int server_sock;
    int client_sock;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    pthread_t thread_id;

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) 
        error_handling("> socket() error");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
        error_handling("> bind() error");

    if (listen(server_sock, 5) == -1)
        error_handling("> listen() error");
    printf("> Echo server started. Waiting for connection on port %d...\n", PORT);

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &client_addr_size);
        if(client_sock == -1)
            error_handling("> accept() error");
        
        printf("> Client connected. IP: %s\n", inet_ntoa(client_addr.sin_addr));

        int *new_sock = (int*)malloc(sizeof(int));
        *new_sock = client_sock;

        if (pthread_create(&thread_id, NULL, handle_client, (void*) new_sock) != 0) {
            close(client_sock);
            free(new_sock);
            fputs("> pthread_create() error\n", stderr);
            continue;
        }

        pthread_detach(thread_id);
        printf("> [Main Thread] Forked thread ID %lu to handle client. \n", (unsigned long)thread_id);
    }

    close(server_sock);

    return 0;
}

void *handle_client(void *arg) {
    int client_sock = *((int*)arg);
    free(arg);

    char message[BUFFER_SIZE];
    int str_len;

    while((str_len = read(client_sock, message, BUFFER_SIZE)) != 0) {
        write(client_sock, message, str_len);
    }

    close(client_sock);
    printf("> [Thread %lu] Client disconnected. Thread terminated.\n", (unsigned long)pthread_self());

    return NULL;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}