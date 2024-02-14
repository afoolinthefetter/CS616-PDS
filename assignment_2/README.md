# OpenMP based concurrent connection handler

This application is responsible for tennis allocation where the server side uses opennp for loop based parallelism to handle connection. On the client side, we read a csv file and create seperate process for each row in the csv file.

## Features

- Reads a file and then created threads to populate waitlist.
- The requests are processed through a sequential unit with the help of mutex lock.
- Threads are there to handle each connection seperately.
- Shell script responsible for running process for each row in <input.csv> file

## Execution
Compile the server
```bash
gcc-13 -fopenmp server.c -o server 
```
Note: I have used gcc-13 specifically, Using lower versions of gcc might opt you some warnings regarding the declaration of functions. These are ignorable here

Run the server
```bash
./server
```

Complie the client
```bash
gcc-13 client.c -o client
```

Run the shellscript to run process for each row in the csv file (One should edit the sh script for cleint-executable filename in case it is other than `client`)
```bash
sh sendrequest.sh <input_file.csv>
```

## Input and Output

#### Sample CSV Input file

| Player-ID | Arrival-time | Gender | Preference |
|-----------|--------------|--------|------------|
| 1         | 1            | M      | D          |
| 2         | 1            | M      | B          |
| 3         | 1            | F      | b          |
| 4         | 1            | F      | S          |


#### Sample Output

| Status |Response                                          |
|--------|-------------------------------------------|
|Failure |Request Timed-out                                  |
|Success |Court Number: 2, Start Time: 57, End Time: 72, Number of Players: 4, Game Type: D|




