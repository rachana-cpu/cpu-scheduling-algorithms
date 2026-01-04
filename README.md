# CPU Scheduling Algorithms Simulator

An implementation of 8 CPU scheduling algorithms.

## Algorithms Implemented

1. FCFS - First Come First Serve
2. SJF - Shortest Job First
3. SRJF - Shortest Remaining Job First
4. Round Robin
5. Priority Scheduling
6. HRRN - Highest Response Ratio Next
7. LJF - Longest Job First
8. LRJF - Longest Remaining Job First

## Compilation

```bash
gcc -o scheduler main.c scheduling.c -lm
```

## Running

```bash
./scheduler.exe
```

## Usage

1. Enter the number of processes
2. For each process, provide:
   - Arrival time
   - Burst time
   - Priority (0 = highest)
3. Select a scheduling algorithm (1-8)
4. For Round Robin, enter time quantum
5. View results and metrics

## Output

For each algorithm, displays:
- Process completion times
- Waiting times
- Turnaround times
- Average metrics
