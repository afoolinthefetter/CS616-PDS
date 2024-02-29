import socket

def send_message_to_server(message):
    # Create a TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Configure the server address
    server_addr = ('127.0.0.1', 8080)

    # Connect to the server
    client_socket.connect(server_addr)

    # Send a message to the server
    client_socket.sendall(message.encode())

    # Receive and print the echoed message from the server
    buffer_size = 1024
    received_data = client_socket.recv(buffer_size)
    print(f"Server says: {received_data.decode()}")

    # Close the client socket when done
    client_socket.close()

if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <message>")
        sys.exit(1)

    # Call the function to send the message to the server
    send_message_to_server(sys.argv[1])
