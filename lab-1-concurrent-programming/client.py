import socket
import csv
import time

# Server configuration
host = '127.0.0.1'
port = 12345

# Create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
s.connect((host, port))

# Read data from the CSV file without pandas
csv_file_path = 'test_case_1.csv'  # Replace with the actual path to your CSV file

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

        # Add a time delay of 1 second
        time.sleep(1)

# Close the socket connection
s.close()
