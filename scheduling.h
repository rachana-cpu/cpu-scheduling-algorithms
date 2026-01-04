#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 50

typedef struct {
    int pid;                    // Process ID
    int arrival_time;          // Arrival time at CPU
    int burst_time;            // Original burst time
    int remaining_time;        // Remaining burst time (for preemptive)
    int priority;              // Priority (lower value = higher priority)
    int completion_time;       // Time when process completes
    int turnaround_time;       // completion_time - arrival_time
    int waiting_time;          // turnaround_time - burst_time
    int response_time;         // First time process gets CPU
} Process;

typedef struct {
    float avg_waiting_time;
    float avg_turnaround_time;
    float avg_response_time;
    float cpu_utilization;
} Metrics;

// Function declarations
void input_processes(Process *processes, int *n);
void display_processes(Process *processes, int n);
void reset_processes(Process *processes, int n);
void calculate_metrics(Process *processes, int n, int total_time, Metrics *metrics);

// Scheduling algorithms
void fcfs(Process *processes, int n);
void sjf(Process *processes, int n);
void srjf(Process *processes, int n);
void round_robin(Process *processes, int n, int time_quantum);
void priority_scheduling(Process *processes, int n);
void hrrn(Process *processes, int n);
void ljf(Process *processes, int n);
void lrjf(Process *processes, int n);

#endif
