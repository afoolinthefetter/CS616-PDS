#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <omp.h>

void handle_connection(int client_socket) {
    // Handle the connection (you can customize this part)
    char message[] = "Hello, client! This is the server.";
    send(client_socket, message, sizeof(message), 0);

    // Close the client socket
    close(client_socket);
}

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // Accept connections and handle them using OpenMP tasks
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            while (1) {
                // Accept a connection
                int client_socket = accept(server_socket, NULL, NULL);
                if (client_socket == -1) {
                    perror("Error accepting connection");
                    close(server_socket);
                    exit(EXIT_FAILURE);
                }

                // Handle the connection in a task
                #pragma omp task
                {
                    handle_connection(client_socket);
                }
            }
        }
    }

    // Close the server socket (this will never be reached in this example)
    close(server_socket);

    return 0;
}
