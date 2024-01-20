import socket
import csv
import time
import sys

# Server configuration
host = '127.0.0.1'
port = 12345

# Check if the CSV file name is provided as a command-line argument
if len(sys.argv) != 2:
    print("Usage: python script.py <csv_file>")
    sys.exit(1)

# Extract the CSV file name from the command-line arguments
csv_file_path = sys.argv[1]

# Create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
s.connect((host, port))

# Send requests to the server serially with a time delay
with open(csv_file_path, 'r') as csvfile:
    csv_reader = csv.DictReader(csvfile)
    for row in csv_reader:
        request_time = row['Request Time']
        request_type = row['Type']
        room = row['Room']
        timeslot = row['Timeslot']

        request = f"{request_time},{request_type},{room},{timeslot}"
        s.sendall(request.encode())
        response = s.recv(1024).decode()

        print(f"Sent: {request}")
        print(f"Received: {response}")
        print()

# Close the socket connection
s.close()
