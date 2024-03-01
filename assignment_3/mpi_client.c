#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mpi.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

void send_message_to_server(const char *message) {
    // Create a TCP socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to the server
    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Send a message to the server
    send(client_socket, message, strlen(message), 0);

    // Receive and print the echoed message from the server
    char received_data[1024];
    recv(client_socket, received_data, sizeof(received_data), 0);
    printf("Server says: %s\n", received_data);

    // Close the client socket when done
    close(client_socket);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Based on the rank, get the line+1 from a csv file
    FILE *file = fopen("input.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        MPI_Finalize();
        return 1;
    }

    char line[1024];
    for (int i = 0; i <= rank; i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fclose(file);
            MPI_Finalize();
            return 1;
        }
    }

    fclose(file);

    // Call the function to send the message to the server
    send_message_to_server(line);

    MPI_Finalize();
    return 0;
}
