#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <omp.h>

void handle_connection(int client_socket, int thread_id) {
    char buffer[1024];
    int bytes_received;
    printf("Thread %d is handling the connection\n", thread_id);
    printf("Going to receive data from the client\n");
    // Receive data from the client
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Echo the received data back to the client
        printf("Waiting for 5 seconds");
        sleep(5);
        send(client_socket, buffer, bytes_received, 0);
    }
    printf("Thread %d is done handling the connection\n", thread_id);
    
    // Close the client socket when done
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Listen for incoming connections
    listen(server_socket, 5);

    printf("Server listening on port 8080...\n");

    // Number of threads to create
    const int num_threads =100;

    // Accept connections in parallel using OpenMP
    #pragma omp parallel num_threads(num_threads)
    {   
        if (omp_get_thread_num() == 0) {
            printf("Number of threads: %d\n", omp_get_num_threads());
        }
        // Each thread has its own copy of the server socket
        int private_server_socket = server_socket;

        // Each thread listens and accepts connections independently
        #pragma omp for
        for (int i = 0; i < num_threads; ++i) {
            // Accept a connection
            client_socket = accept(private_server_socket, (struct sockaddr*)&client_addr, &client_len);

            // Handle the connection in a separate function
            handle_connection(client_socket, omp_get_thread_num());
        }
    }

    // Close the server socket when done
    close(server_socket);

    return 0;
}
