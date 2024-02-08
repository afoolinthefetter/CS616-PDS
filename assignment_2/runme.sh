#!/bin/bash

# Path to the executable
executable_path="./client"
# Number of times to run the executable
num_runs=10

# open -a Terminal "./server"

#Run the server
# $executable_server_path &

# Loop to run the executable multiple times
for ((i=1; i<=$num_runs; i++))
do
    echo "Running iteration $i..."
    $executable_path &  # Add any command line arguments here
    echo "------------------"
done

echo "Script finished!"
