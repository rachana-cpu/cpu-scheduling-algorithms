#include "scheduling.h"
#include <limits.h>

void display_menu() {
    printf("\n");
    printf("================================================\n");
    printf("   CPU SCHEDULING ALGORITHMS SIMULATOR\n");
    printf("================================================\n");
    printf("1. First Come First Serve (FCFS)\n");
    printf("2. Shortest Job First (SJF)\n");
    printf("3. Shortest Remaining Job First (SRJF)\n");
    printf("4. Round Robin (RR)\n");
    printf("5. Priority Scheduling\n");
    printf("6. Highest Response Ratio Next (HRRN)\n");
    printf("7. Longest Job First (LJF)\n");
    printf("8. Longest Remaining Job First (LRJF)\n");
    printf("9. Exit\n");
    printf("================================================\n");
}

void run_algorithm(int choice, Process *processes, int n) {
    Process *temp_processes = (Process *)malloc(n * sizeof(Process));
    
    // Copy processes to temporary array
    for (int i = 0; i < n; i++) {
        temp_processes[i] = processes[i];
    }
    
    int total_time = 0;
    char algorithm_name[50];
    
    switch (choice) {
        case 1:
            strcpy(algorithm_name, "First Come First Serve (FCFS)");
            fcfs(temp_processes, n);
            break;
        case 2:
            strcpy(algorithm_name, "Shortest Job First (SJF)");
            sjf(temp_processes, n);
            break;
        case 3:
            strcpy(algorithm_name, "Shortest Remaining Job First (SRJF)");
            srjf(temp_processes, n);
            break;
        case 4: {
            int time_quantum;
            printf("\nEnter time quantum: ");
            scanf("%d", &time_quantum);
            strcpy(algorithm_name, "Round Robin (RR)");
            round_robin(temp_processes, n, time_quantum);
            break;
        }
        case 5:
            strcpy(algorithm_name, "Priority Scheduling");
            priority_scheduling(temp_processes, n);
            break;
        case 6:
            strcpy(algorithm_name, "Highest Response Ratio Next (HRRN)");
            hrrn(temp_processes, n);
            break;
        case 7:
            strcpy(algorithm_name, "Longest Job First (LJF)");
            ljf(temp_processes, n);
            break;
        case 8:
            strcpy(algorithm_name, "Longest Remaining Job First (LRJF)");
            lrjf(temp_processes, n);
            break;
        default:
            printf("Invalid choice!\n");
            free(temp_processes);
            return;
    }
    
    // Calculate total time
    for (int i = 0; i < n; i++) {
        if (temp_processes[i].completion_time > total_time) {
            total_time = temp_processes[i].completion_time;
        }
    }
    
    // Display results
    printf("\n================================================\n");
    printf("  %s\n", algorithm_name);
    printf("================================================\n");
    display_processes(temp_processes, n);
    
    // Calculate and display metrics
    Metrics metrics;
    calculate_metrics(temp_processes, n, total_time, &metrics);
    
    printf("\n================================================\n");
    printf("                   METRICS\n");
    printf("================================================\n");
    printf("Average Waiting Time:     %.2f units\n", metrics.avg_waiting_time);
    printf("Average Turnaround Time:  %.2f units\n", metrics.avg_turnaround_time);
    printf("Average Response Time:    %.2f units\n", metrics.avg_response_time);
    printf("Total Execution Time:     %d units\n", total_time);
    printf("================================================\n");
    
    free(temp_processes);
}

int main() {
    Process processes[MAX_PROCESSES];
    int n = 0;
    int choice;
    int has_input = 0;
    
    printf("\n");
    printf("================================================\n");
    printf("   CPU SCHEDULING ALGORITHMS SIMULATOR\n");
    printf("================================================\n");
    
    // Get initial processes
    input_processes(processes, &n);
    has_input = 1;
    
    printf("\n\nInitial Processes:\n");
    display_processes(processes, n);
    
    while (1) {
        display_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        if (choice == 9) {
            printf("\nThank you for using CPU Scheduling Simulator!\n");
            break;
        }
        
        if (choice >= 1 && choice <= 8) {
            if (has_input) {
                run_algorithm(choice, processes, n);
            } else {
                printf("Please enter processes first!\n");
            }
        } else {
            printf("Invalid choice! Please try again.\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar(); // consume newline
        getchar(); // wait for user input
    }
    
    return 0;
}
