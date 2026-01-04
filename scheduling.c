#include "scheduling.h"

void input_processes(Process *processes, int *n) {
    printf("\nEnter number of processes: ");
    scanf("%d", n);
    
    for (int i = 0; i < *n; i++) {
        processes[i].pid = i + 1;
        printf("\nProcess %d:\n", i + 1);
        printf("  Arrival time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("  Burst time: ");
        scanf("%d", &processes[i].burst_time);
        printf("  Priority (0=highest): ");
        scanf("%d", &processes[i].priority);
        
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
        processes[i].response_time = -1;
    }
}

void display_processes(Process *processes, int n) {
    printf("\n%-5s %-10s %-10s %-10s %-10s %-15s %-15s\n",
           "PID", "Arrival", "Burst", "Priority", "Complete", "Turnaround", "Waiting");
    printf("==========================================================================================\n");
    
    for (int i = 0; i < n; i++) {
        printf("%-5d %-10d %-10d %-10d %-10d %-15d %-15d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);
    }
}

void reset_processes(Process *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completion_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].waiting_time = 0;
        processes[i].response_time = -1;
    }
}

void calculate_metrics(Process *processes, int n, int total_time, Metrics *metrics) {
    float total_waiting = 0, total_turnaround = 0, total_response = 0;
    
    for (int i = 0; i < n; i++) {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        
        total_waiting += processes[i].waiting_time;
        total_turnaround += processes[i].turnaround_time;
        if (processes[i].response_time != -1) {
            total_response += processes[i].response_time;
        }
    }
    
    metrics->avg_waiting_time = total_waiting / n;
    metrics->avg_turnaround_time = total_turnaround / n;
    metrics->avg_response_time = total_response / n;
    metrics->cpu_utilization = ((float)total_time / total_time) * 100;
}

// FCFS - First Come First Serve (Non-preemptive)
void fcfs(Process *processes, int n) {
    reset_processes(processes, n);
    
    // Sort by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
    
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        
        processes[i].response_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].completion_time = current_time;
    }
}

// SJF - Shortest Job First (Non-preemptive)
void sjf(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int shortest_idx = -1;
        int shortest_time = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].completion_time == 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].burst_time < shortest_time) {
                shortest_idx = i;
                shortest_time = processes[i].burst_time;
            }
        }
        
        if (shortest_idx == -1) {
            // No process arrived yet
            for (int i = 0; i < n; i++) {
                if (processes[i].completion_time == 0) {
                    current_time = processes[i].arrival_time;
                    shortest_idx = i;
                    break;
                }
            }
        }
        
        if (shortest_idx != -1) {
            processes[shortest_idx].response_time = current_time - processes[shortest_idx].arrival_time;
            current_time += processes[shortest_idx].burst_time;
            processes[shortest_idx].completion_time = current_time;
            completed++;
        }
    }
}

// SRJF - Shortest Remaining Job First (Preemptive)
void srjf(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int shortest_idx = -1;
        int shortest_remaining = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].remaining_time < shortest_remaining) {
                shortest_idx = i;
                shortest_remaining = processes[i].remaining_time;
            }
        }
        
        if (shortest_idx == -1) {
            // No process arrived yet
            int min_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && 
                    processes[i].arrival_time < min_arrival) {
                    min_arrival = processes[i].arrival_time;
                    shortest_idx = i;
                }
            }
            
            if (shortest_idx != -1) {
                current_time = processes[shortest_idx].arrival_time;
            }
        }
        
        if (shortest_idx != -1) {
            if (processes[shortest_idx].response_time == -1) {
                processes[shortest_idx].response_time = current_time - processes[shortest_idx].arrival_time;
            }
            
            processes[shortest_idx].remaining_time--;
            current_time++;
            
            if (processes[shortest_idx].remaining_time == 0) {
                processes[shortest_idx].completion_time = current_time;
                completed++;
            }
        }
    }
}

// Round Robin (Preemptive)
void round_robin(Process *processes, int n, int time_quantum) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    int *queue = (int *)malloc(n * sizeof(int));
    int queue_front = 0, queue_rear = 0;
    int *in_queue = (int *)malloc(n * sizeof(int));
    memset(in_queue, 0, n * sizeof(int));
    
    // Initially add all arrived processes
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time == 0) {
            queue[queue_rear++] = i;
            in_queue[i] = 1;
        }
    }
    
    while (completed < n) {
        if (queue_front == queue_rear) {
            // Queue is empty, find next arriving process
            int min_arrival = INT_MAX;
            int next_process = -1;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && 
                    processes[i].arrival_time > current_time &&
                    processes[i].arrival_time < min_arrival) {
                    min_arrival = processes[i].arrival_time;
                    next_process = i;
                }
            }
            
            if (next_process != -1) {
                current_time = processes[next_process].arrival_time;
                queue[queue_rear++] = next_process;
                in_queue[next_process] = 1;
            }
        }
        
        if (queue_front < queue_rear) {
            int idx = queue[queue_front++];
            
            if (processes[idx].response_time == -1) {
                processes[idx].response_time = current_time - processes[idx].arrival_time;
            }
            
            int time_used = (processes[idx].remaining_time < time_quantum) ? 
                            processes[idx].remaining_time : time_quantum;
            
            processes[idx].remaining_time -= time_used;
            current_time += time_used;
            
            if (processes[idx].remaining_time == 0) {
                processes[idx].completion_time = current_time;
                completed++;
            } else {
                // Add back to queue
                queue[queue_rear++] = idx;
            }
            
            // Add newly arrived processes
            for (int i = 0; i < n; i++) {
                if (processes[i].arrival_time <= current_time && 
                    processes[i].remaining_time > 0 && 
                    in_queue[i] == 0) {
                    queue[queue_rear++] = i;
                    in_queue[i] = 1;
                }
            }
        }
    }
    
    free(queue);
    free(in_queue);
}

// Priority Scheduling (Non-preemptive)
void priority_scheduling(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int highest_priority_idx = -1;
        int highest_priority = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].completion_time == 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].priority < highest_priority) {
                highest_priority_idx = i;
                highest_priority = processes[i].priority;
            }
        }
        
        if (highest_priority_idx == -1) {
            // No process arrived yet
            for (int i = 0; i < n; i++) {
                if (processes[i].completion_time == 0) {
                    current_time = processes[i].arrival_time;
                    highest_priority_idx = i;
                    break;
                }
            }
        }
        
        if (highest_priority_idx != -1) {
            processes[highest_priority_idx].response_time = current_time - processes[highest_priority_idx].arrival_time;
            current_time += processes[highest_priority_idx].burst_time;
            processes[highest_priority_idx].completion_time = current_time;
            completed++;
        }
    }
}

// HRRN - Highest Response Ratio Next (Non-preemptive)
void hrrn(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int best_idx = -1;
        double highest_ratio = -1;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].completion_time == 0 && 
                processes[i].arrival_time <= current_time) {
                double waiting_time = current_time - processes[i].arrival_time;
                double response_ratio = (waiting_time + processes[i].burst_time) / (double)processes[i].burst_time;
                
                if (response_ratio > highest_ratio) {
                    highest_ratio = response_ratio;
                    best_idx = i;
                }
            }
        }
        
        if (best_idx == -1) {
            // No process arrived yet
            for (int i = 0; i < n; i++) {
                if (processes[i].completion_time == 0) {
                    current_time = processes[i].arrival_time;
                    best_idx = i;
                    break;
                }
            }
        }
        
        if (best_idx != -1) {
            processes[best_idx].response_time = current_time - processes[best_idx].arrival_time;
            current_time += processes[best_idx].burst_time;
            processes[best_idx].completion_time = current_time;
            completed++;
        }
    }
}

// LJF - Longest Job First (Non-preemptive)
void ljf(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int longest_idx = -1;
        int longest_time = -1;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].completion_time == 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].burst_time > longest_time) {
                longest_idx = i;
                longest_time = processes[i].burst_time;
            }
        }
        
        if (longest_idx == -1) {
            // No process arrived yet
            for (int i = 0; i < n; i++) {
                if (processes[i].completion_time == 0) {
                    current_time = processes[i].arrival_time;
                    longest_idx = i;
                    break;
                }
            }
        }
        
        if (longest_idx != -1) {
            processes[longest_idx].response_time = current_time - processes[longest_idx].arrival_time;
            current_time += processes[longest_idx].burst_time;
            processes[longest_idx].completion_time = current_time;
            completed++;
        }
    }
}

// LRJF - Longest Remaining Job First (Preemptive)
void lrjf(Process *processes, int n) {
    reset_processes(processes, n);
    
    int completed = 0;
    int current_time = 0;
    
    while (completed < n) {
        int longest_idx = -1;
        int longest_remaining = -1;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].remaining_time > longest_remaining) {
                longest_idx = i;
                longest_remaining = processes[i].remaining_time;
            }
        }
        
        if (longest_idx == -1) {
            // No process arrived yet
            int min_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && 
                    processes[i].arrival_time < min_arrival) {
                    min_arrival = processes[i].arrival_time;
                    longest_idx = i;
                }
            }
            
            if (longest_idx != -1) {
                current_time = processes[longest_idx].arrival_time;
            }
        }
        
        if (longest_idx != -1) {
            if (processes[longest_idx].response_time == -1) {
                processes[longest_idx].response_time = current_time - processes[longest_idx].arrival_time;
            }
            
            processes[longest_idx].remaining_time--;
            current_time++;
            
            if (processes[longest_idx].remaining_time == 0) {
                processes[longest_idx].completion_time = current_time;
                completed++;
            }
        }
    }
}
