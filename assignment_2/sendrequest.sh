#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <csv_file>"
    exit 1
fi

filename="$1"
client_executable="./demo_client1"


tail -n +2 "$filename" | while IFS=, read -r player_id arrival_time gender preference; do

    message="$player_id,$arrival_time,$gender,$preference"

    "$client_executable" "$message" &

    sleep 1

done