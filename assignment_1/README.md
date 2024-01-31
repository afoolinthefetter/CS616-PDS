# Assignment 1

Concurrent Tennis Court Allocator.

## Features

- Reads a file and then created threads to populate waitlist.
- The requests are processed through a sequential unit with the help of mutex lock.
- Produces an *output.txt* file for the allocations.

## Execution
```python
python3 bookings.py input.csv
```

## Input and Output
Characteristics of the player:
- Gender: M or F
- Game Preference: S, D, b, B 
  - S -> Singles (2 players),
  - D -> Doubles (4 players),
  - b -> first preference S and second preference D
  - B -> first preference D and second preference S

#### Sample Input

| Player-ID | Arrival-time | Gender | Preference |
|-----------|--------------|--------|------------|
| 1         | 1            | M      | D          |
| 2         | 1            | M      | B          |
| 3         | 1            | F      | b          |
| 4         | 1            | F      | S          |


#### Sample Output

| Start-Time | End-Time | Players         | Court-Number |
|------------|----------|-----------------|--------------|
| 4          | 19       | 5, 9            | 1            |
| 5          | 20       | 11, 13          | 2            |
| 12         | 27       | 1, 3, 7, 17     | 3            |


