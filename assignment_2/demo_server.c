#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345

void start_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // Accept a connection from a client
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));

        // Receive the message from the client
        char buffer[1024];
        ssize_t recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (recv_size > 0) {
            buffer[recv_size] = '\0';  // Null-terminate the received data
            printf("Received message from client: %s\n", buffer);

            // Send a reply back to the client
            char reply_message[1024];
            snprintf(reply_message, sizeof(reply_message), "Server received: %s", buffer);
            send(client_socket, reply_message, strlen(reply_message), 0);
        }

        // Close the connection with the client
        close(client_socket);
    }

    close(server_socket);
}

int main() {
    // Call the function to start the server
    start_server();

    return 0;
}
