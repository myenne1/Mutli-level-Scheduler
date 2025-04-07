#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "prioque.h"

#define MAX_BLOCKED 11

typedef struct _Process
{
    int time;
    int pid;
    int originalRunTime;
    int remainingRunTime;
    int io;
    int repeat;
    int ioEndTime;
    int gCounter;
    Queue queueLevel;
} Process;

// Globals
Process processes[11];
Process blockedProcesses[MAX_BLOCKED];
Queue blockedProcesses;
Queue q1, q2, q3, q4;
int runningProcessCount = 0;
int currentTime = 0;
int blockedCount = 0;

// Function to add process to blockedProcesses array
void sendToIo(Process p)
{
    if (blockedCount < MAX_BLOCKED)
    {
        blockedProcesses[blockedCount] = p;
        blockedCount++;
        printf("I/O: Process %d blocked for I/O at time %d.\n", p.pid, currentTime);
    }
    else
    {
        printf("Blocked Processes array reached limit\n");
    }
}

void processQ1()
{
    int q = 10;
    int b = 1;
    int usedTime = 0;
    Process p;

    remove_from_front(&q1, &p);

    if (p.remainingRunTime <= q)
    {
        printf("RUN: Process %d started execution from level 1 at time %d; wants to execute for %d ticks.\n", p.pid, currentTime, p.remainingRunTime);
        if (p.remainingRunTime < q)
        {
            usedTime = p.remainingRunTime; // ensures currentTime is accurate and run times aren't negative
            p.remainingRunTime = 0;
            currentTime += usedTime;
        }
        else
        {
            p.remainingRunTime -= q;
            currentTime += q;
        }

        if (p.io == 0 && p.repeat == 0) // Checks if process is finished
        {
            runningProcessCount -= 1;
            printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
        }
        else
        {
            p.ioEndTime = currentTime + p.io; // Moves to I/O if not finished
            p.queueLevel = q1;
            sendToIo(p);
        }
    }
    else
    { // Runs and demotes to level 2
        p.remainingRunTime -= q;
        currentTime += q;

        add_to_queue(&q2, &p, p.pid);

        processQ2();
    }
}

void processQ2()
{
    int q = 30; 
    int usedTime = 0;
    Process p;

    if (empty_queue(&q1))
    {
        remove_from_front(&q2, &p);
        p.queueLevel = 2;  
        
        printf("RUN: Process %d started execution from level 1 at time %d; wants to execute for %d ticks.\n", p.pid, currentTime, p.remainingRunTime);
        
        if (p.remainingRunTime <= q)  // Process finished within the quantum time
        {
            if (p.remainingRunTime < q)
            {
                usedTime = p.remainingRunTime;
                p.remainingRunTime = 0;
                currentTime += usedTime;
            }
            else
            {
                p.remainingRunTime = 0;
                currentTime += q;
            }
            
            p.g_counter++;
            p.b_counter = 0;
            
            if (p.g_counter >= 1) //Check if we need to promote
            {
                p.currentQueue = 1; 
                p.g_counter = 0;
                add_to_queue(&q1, &p, p.pid);
            }
            else if (p.io == 0 && p.repeat == 0)  // Process is done and does not need Io or Repeats
            {
                runningProcessCount--;
                printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
            }
            else //else if we need IO
            {
                p.ioEndTime = currentTime + p.io;
                sendToIo(p);
            }
        }
        else  // else if process cannot finish within quantum
        { 
            p.remainingRunTime -= q;
            currentTime += q;
            
            p.b_counter++;
            p.g_counter = 0;
            
            if (p.b_counter >= 2)
            {
                p.currentQueue = 3; 
                p.b_counter = 0;

                add_to_queue(&q3, &p, p.pid);
            }
            else
            {
                // Stay in same queue
                add_to_queue(&q2, &p, p.pid);
                printf("QUEUED: Process %d queued at level 2 at time %d.\n", p.pid, currentTime);
            }
        }
    }
}




int main(int argc, char *argv[])
{
    Queue readyQueue;
    Process p;
    int count = 0;

    printf("Queue entries one per line: \n");

    while (scanf("%d %d %d %d %d", &processes[count].time,
                 &processes[count].pid,
                 &processes[count].originalRunTime,
                 &processes[count].io,
                 &processes[count].repeat) == 5)
    {
        count++;
        runningProcessCount++;
    }

    // Initialize queues
    init_queue(&readyQueue, sizeof(Process), TRUE, NULL, FALSE);

    init_queue(&q1, sizeof(Process), TRUE, NULL, FALSE); // q1 = highest priority
    init_queue(&q2, sizeof(Process), TRUE, NULL, FALSE);
    init_queue(&q3, sizeof(Process), TRUE, NULL, FALSE);
    init_queue(&q4, sizeof(Process), TRUE, NULL, FALSE);

    // Start simulation
    while (runningProcessCount != 0)
    {
        for (int i = 0; i < count; i++) // checks if any processes arrived
        {
            if (processes[i].time <= currentTime)
            {
                printf("PID: %d Arrival Time: %d\n", processes[i].pid, currentTime);
                add_to_queue(&readyQueue, &processes[i], processes[i].pid);
                printf("CREATE: Process %d entered the ready queue at time %d.\n", processes[i].pid, currentTime);

                for (int j = i; j < count - 1; j++)
                { // Removes process from processes array by shifting all elements after it to the left
                    processes[j] = processes[j + 1];
                }
                count--;
                i--;
            }
        }

        for (int i = 0; i < blockedCount; i++)
        {
            if (blockedProcesses[i].ioEndTime <= currentTime)
            {
                blockedProcesses[i].remainingRunTime = blockedProcesses[i].originalRunTime; // Reset run time for repeat
                blockedProcesses[i].repeat -= 1;
                add_to_queue(&p.queueLevel, &blockedProcesses[i], &blockedProcesses[i].pid);

                for (int j = i; j < blockedCount - 1; j++)
                { // Removes process from blockedProcesses array by shifting all elements after it to the left
                    blockedProcesses[j] = blockedProcesses[j + 1];
                }
                blockedCount--;
            }
        }
        if (!empty_queue(&readyQueue))
        {
            remove_from_front(&readyQueue, &p);
            add_to_queue(&q1, &p, p.pid);
            processQ1();
        }
        currentTime++;
    }
}
