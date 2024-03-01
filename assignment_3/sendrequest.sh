#!/bin/bash

# Replace 'your_file.csv' with the actual CSV file path
csv_file="input.csv"

# Get the number of lines in the CSV file
number_of_requests=$(wc -l < "$csv_file")

# Run the MPI command with the count
mpiexec -n "$number_of_requests" --oversubscribe python3 client.py
