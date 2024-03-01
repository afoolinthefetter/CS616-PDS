# MPI communication on client side and OpenMP based concurrent connection handler 

This application is responsible for tennis allocation where the server side uses opennp for loop based parallelism to handle connection. On the client side, we read a csv file and create seperate process for each row in the csv file. At the end, the players who lost the game will wish the winner Congractulations and the winner will reply with a thank you message.

## Features

- Makes MPI processes for the requests
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

Complie the client explicitly
We need mpi4py library to run the client side of the program.

Create Virtual Environment
```bash
python3 -m virtualenv venv
```

Activate Virtual Environment
```bash
source venv/bin/activate
```

Pip install mpi4py
```bash
pip3 install mpi4py
```

MPI Run the client
```bash
mpiexec -n <number of requests> --oversubscribe python3 client.py
```

The 
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


#### Sample Output for reply from the server

| Status |Response                                          |
|--------|-------------------------------------------|
|Failure |Request Timed-out                                  |
|Success |Court Number: 2, Start Time: 57, End Time: 72, Winner: 12,15 ,Number of Players: 4, Game Type: D|
|Success |Court Number: 3, Start Time: 17, End Time: 22, Winner: 6 ,Number of Players: 2, Game Type: S|



#### Sample Output for Congratulations



