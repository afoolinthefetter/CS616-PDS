#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void send_message_to_server(const char *message) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create a TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address
    server_addr.sin_port = htons(8080);

    // Connect to the server
    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Send a message to the server
    send(client_socket, message, strlen(message), 0);

    // Receive and print the echoed message from the server
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Server says: %s\n", buffer);

    // Close the client socket when done
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Call the function to send the message to the server
    send_message_to_server(argv[1]);

    return 0;
}
