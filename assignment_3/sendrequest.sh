#!/bin/env python3

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <csv_file>"
    exit 1
fi

filename="$1"
python_script="client.py"

tail -n +2 "$filename" | while IFS=, read -r player_id arrival_time gender preference; do

    message="$player_id,$arrival_time,$gender,$preference"
    echo  p"$client_executable" "$message"
    python3 "$python_script" "$message" &

    sleep 1

done
