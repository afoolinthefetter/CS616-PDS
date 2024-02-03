# Assignment 1

Concurrent Client Service Application responsible for managing classroom booking requests.

## Features

- Starts a server to handle connections from multiple clients.
- Each client reads from a csv file and sends a nunch of booking requestes to the server.
- Threads are there to handle each connection seperately.
- Based on the available slot the requests are processed and rooms are allocated.

## Execution
Start the server
```python
python3 server.py
```
Start a single client and pass it a csv file to send it to the server. If you need multiple clients ti send requests from multiple files, just set up more clients in the same way.
```python
python3 client.py <input_file.csv>
```


## Input and Output
Characteristics of the program:
- Bookings will be done for a day for 5 classrooms.
- Timeslots are of 1.5 hours starting from 08:00 - 20:00. They do not overlap.
- Cancel request will fail if made within 20 seconds from the time of booking.
- In the test case, every second one request is made. The Request Time column is the relative time in seconds from start of program.

### Status Codes

| Status Code | Status   | 
|--------------|--------|
| 1            | OK   |
| 2            | Slot Already booked   | 
| 3            | Cancel request received within the cool down period |
| 4            | Invalid Request   |
#### Sample Input

| Request Time | Type   | Room | Timeslot     |
|--------------|--------|------|--------------|
| 1            | BOOK   | 4    | 17:00-18:30  |
| 2            | BOOK   | 1    | 14:00-15:30  |
| 3            | CANCEL | 5    | 18:30-20:00  |
| 4            | BOOK   | 4    | 14:00-15:30  |


#### Sample Output

| Request Time | Type   | Room | Timeslot     | Status |
|--------------|--------|------|--------------|--------|
| 1            | BOOK   | 4    | 17:00-18:30  | 0      |
| 2            | BOOK   | 1    | 14:00-15:30  | 0      |
| 3            | CANCEL | 5    | 18:30-20:00  | -3     |
| 4            | BOOK   | 4    | 14:00-15:30  | 0      |
| 5            | BOOK   | 4    | 15:30-17:00  | 0      |
| 6            | CANCEL | 5    | 12:30-14:00  | -3     |
| 7            | BOOK   | 5    | 11:00-12:30  | 0      |
| 8            | BOOK   | 3    | 11:00-12:30  | 0      |
| 9            | CANCEL | 1    | 14:00-15:30  | -2     |



