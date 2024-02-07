#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[] = "Hello, Server!";
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

    return 0;
}
