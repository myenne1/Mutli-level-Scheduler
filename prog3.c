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
    int remainingIoTime;
    int originalIoTime;
    int repeat;
    int ioEndTime;
    int gCounter;
    int bCounter;
    int queueLevel;
    int cpuTimeUsed;
} Process;

// Globals
Process processes[11];
Process blockedProcesses[MAX_BLOCKED];
Queue q1, q2, q3, q4;
Queue finishedProcesses;
int runningProcessCount = 0;
int currentTime = 0;
int blockedCount = 0;
int finishedProcessCount;
int nullProcessTime = 0;

// Forward declaration of processQ2
void processQ2();

int compare_processes(const void *e1, const void *e2)
{
    const Process *p1 = (const Process *)e1;
    const Process *p2 = (const Process *)e2;

    // Consider processes equal if their PID matches
    return p1->pid != p2->pid;
}

// Function to add process to blockedProcesses array
void sendToIo(Process p)
{
    if (blockedCount < MAX_BLOCKED)
    {
        blockedProcesses[blockedCount] = p;
        blockedProcesses[blockedCount].queueLevel = p.queueLevel;
        blockedProcesses[blockedCount].ioEndTime = currentTime + p.remainingIoTime;
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
            p.cpuTimeUsed = usedTime;
            currentTime += usedTime;
        }
        else
        {
            p.remainingRunTime -= q;
            p.cpuTimeUsed += q;
            currentTime += q;
        }

        if (p.remainingIoTime == 0 && p.repeat == 0 && p.remainingRunTime == 0) // Checks if process is finished
        {
            runningProcessCount -= 1;
            printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
            add_to_queue(&finishedProcesses, &p, p.pid);
        }
        else if (p.remainingRunTime == 0)
        {
            sendToIo(p);
        }
    }
    else
    { // Runs and demotes to level 2
        p.remainingRunTime -= q;
        currentTime += q;

        add_to_queue(&q2, &p, p.pid);
    }
}

void processQ2()
{
    int q = 30;
    Process p;

    if (empty_queue(&q1))
    {
        remove_from_front(&q2, &p);

        printf("RUN: Process %d started execution from level 2 at time %d; wants to execute for %d ticks.\n",
               p.pid, currentTime, p.remainingRunTime);

        if (p.remainingRunTime <= q)
        {
            currentTime += p.remainingRunTime;
            p.cpuTimeUsed += p.remainingRunTime;
            p.remainingRunTime = 0;

            p.gCounter++;
            p.bCounter = 0;

            if (p.gCounter >= 1)
            {
                p.queueLevel = 1;
                p.gCounter = 0;
            }

            if (p.repeat == 0 && p.remainingIoTime == 0) // Checks if process is finished
            {
                runningProcessCount--;
                printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
                add_to_queue(&finishedProcesses, &p, p.pid);
            }
            else if (p.remainingIoTime > 0) // Moves to I/O if not finished
            {
                sendToIo(p);
            }
            else
            {
                if (p.queueLevel == 1)
                {
                    add_to_queue(&q1, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 1 at time %d.\n", p.pid, currentTime);
                }
                else
                {
                    add_to_queue(&q2, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 2 at time %d.\n", p.pid, currentTime);
                }
            }
        }
        else
        {
            p.remainingRunTime -= q;
            p.cpuTimeUsed += q;
            currentTime += q;

            p.bCounter++;
            p.gCounter = 0;

            if (p.bCounter >= 2)
            {
                p.queueLevel = 3;
                p.bCounter = 0;
                add_to_queue(&q3, &p, p.pid);
                printf("QUEUED: Process %d queued at level 3 at time %d.\n", p.pid, currentTime);
            }
            else
            {
                add_to_queue(&q2, &p, p.pid);
                printf("QUEUED: Process %d queued at level 2 at time %d.\n", p.pid, currentTime);
            }
        }
    }
}

void processQ3()
{
    int q = 100;
    Process p;

    if (empty_queue(&q1) && empty_queue(&q2))
    {
        remove_from_front(&q3, &p);

        printf("RUN: Process %d started execution from level 3 at time %d; wants to execute for %d ticks.\n",
               p.pid, currentTime, p.remainingRunTime);

        if (p.remainingRunTime <= q)
        {
            currentTime += p.remainingRunTime;
            p.cpuTimeUsed += p.remainingRunTime;
            p.remainingRunTime = 0;

            p.gCounter++;
            p.bCounter = 0;

            if (p.gCounter >= 1)
            {
                p.queueLevel = 2;
                p.gCounter = 0;
            }

            if (p.repeat == 0 && p.remainingIoTime == 0)
            {
                runningProcessCount--;
                printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
                add_to_queue(&finishedProcesses, &p, p.pid);
            }
            else if (p.remainingIoTime > 0)
            {
                sendToIo(p);
            }
            else
            {
                if (p.queueLevel == 2)
                {
                    add_to_queue(&q2, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 2 at time %d.\n", p.pid, currentTime);
                }
                else
                {
                    add_to_queue(&q3, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 3 at time %d.\n", p.pid, currentTime);
                }
            }
        }
        else
        {
            p.remainingRunTime -= q;
            p.cpuTimeUsed += q;
            currentTime += q;

            p.bCounter++;
            p.gCounter = 0;

            if (p.bCounter >= 2)
            {
                p.queueLevel = 4;
                p.bCounter = 0;
                add_to_queue(&q4, &p, p.pid);
                printf("QUEUED: Process %d queued at level 4 at time %d.\n", p.pid, currentTime);
            }
            else
            {
                add_to_queue(&q3, &p, p.pid);
                printf("QUEUED: Process %d queued at level 3 at time %d.\n", p.pid, currentTime);
            }
        }
    }
}

void processQ4()
{
    int q = 200;
    Process p;

    if (empty_queue(&q1) && empty_queue(&q2) && empty_queue(&q3))
    {
        remove_from_front(&q4, &p);

        printf("RUN: Process %d started execution from level 4 at time %d; wants to execute for %d ticks.\n",
               p.pid, currentTime, p.remainingRunTime);

        if (p.remainingRunTime <= q)
        {
            currentTime += p.remainingRunTime;
            p.cpuTimeUsed += p.remainingRunTime;
            p.remainingRunTime = 0;

            p.gCounter++;
            p.bCounter = 0;

            if (p.gCounter >= 1)
            {
                p.queueLevel = 3;
                p.gCounter = 0;
            }

            if (p.repeat == 0 && p.remainingIoTime == 0)
            {
                runningProcessCount--;
                printf("FINISHED: Process %d finished at time %d.\n", p.pid, currentTime);
                add_to_queue(&finishedProcesses, &p, p.pid);
            }
            else if (p.remainingIoTime > 0)
            {
                sendToIo(p);
            }
            else
            {
                if (p.queueLevel == 3)
                {
                    add_to_queue(&q3, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 3 at time %d.\n", p.pid, currentTime);
                }
                else
                {
                    add_to_queue(&q4, &p, p.pid);
                    printf("QUEUED: Process %d queued at level 4 at time %d.\n", p.pid, currentTime);
                }
            }
        }
        else
        {
            p.remainingRunTime -= q;
            p.cpuTimeUsed += q;
            currentTime += q;

            p.bCounter++;
            p.gCounter = 0;

            add_to_queue(&q4, &p, p.pid);
            printf("QUEUED: Process %d queued at level 4 at time %d.\n", p.pid, currentTime);
        }
    }
}

int main(int argc, char *argv[])
{
    Queue readyQueue;
    Process p;
    int count = 0;

    printf("Queue entries one per line: \n");

    char buffer[256];
    while (count < 11 && fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        if (buffer[0] == '\n' || buffer[0] == '\0' || buffer[0] == ' ')
            break;

        Process newProcess;
        int fieldsRead = sscanf(buffer, "%d %d %d %d %d",
                                &newProcess.time,
                                &newProcess.pid,
                                &newProcess.originalRunTime,
                                &newProcess.originalIoTime,
                                &newProcess.repeat);

        if (fieldsRead != 5)
        {
            break;
        }

        newProcess.remainingRunTime = newProcess.originalRunTime;
        newProcess.remainingIoTime = newProcess.originalIoTime;
        newProcess.gCounter = 0;
        newProcess.bCounter = 0;
        newProcess.queueLevel = 1; // Start all processes at level 1

        processes[count++] = newProcess;
        runningProcessCount++;
    }

    // Initialize queues
    init_queue(&readyQueue, sizeof(Process), TRUE, NULL, FALSE);
    init_queue(&finishedProcesses, sizeof(Process), FALSE, compare_processes, TRUE);

    init_queue(&q1, sizeof(Process), TRUE, NULL, FALSE); // q1 = highest priority
    init_queue(&q2, sizeof(Process), TRUE, NULL, FALSE);
    init_queue(&q3, sizeof(Process), TRUE, NULL, FALSE);
    init_queue(&q4, sizeof(Process), TRUE, NULL, FALSE);

    // Start simulation
    while (runningProcessCount > 0 || count > 0 || blockedCount > 0)
    {
        for (int i = 0; i < count; i++) // checks if any processes arrived
        {
            if (processes[i].time <= currentTime)
            {
                printf("PID: %d, ARRIVAL TIME: %d\n", processes[i].pid, currentTime);
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
                blockedProcesses[i].remainingRunTime = blockedProcesses[i].originalRunTime; // Reset run time and I/O time for repeat
                blockedProcesses[i].remainingIoTime = 0;
                if (blockedProcesses[i].repeat > 0)
                {
                    blockedProcesses[i].repeat -= 1;
                }

                // add_to_queue(&readyQueue, &blockedProcesses[i], blockedProcesses[i].pid);
                int level = blockedProcesses[i].queueLevel;

                if (level == 1)
                {
                    add_to_queue(&q1, &blockedProcesses[i], blockedProcesses[i].pid);
                }
                else if (level == 2)
                {
                    add_to_queue(&q2, &blockedProcesses[i], blockedProcesses[i].pid);
                }
                else if (level == 3)
                {
                    add_to_queue(&q3, &blockedProcesses[i], blockedProcesses[i].pid);
                }
                else
                {
                    add_to_queue(&q4, &blockedProcesses[i], blockedProcesses[i].pid);
                }

                for (int j = i; j < blockedCount - 1; j++)
                { // Removes process from blockedProcesses array by shifting all elements after it to the left
                    blockedProcesses[j] = blockedProcesses[j + 1];
                }
                blockedCount--;
                i--;
            }
        }

        if (!empty_queue(&readyQueue))
        {
            remove_from_front(&readyQueue, &p);
            add_to_queue(&q1, &p, p.pid);
            p.queueLevel = 1;
        }

        if (!empty_queue(&q1))
        {
            processQ1();
        }
        else if (!empty_queue(&q2))
        {
            processQ2();
        }
        else if (!empty_queue(&q3))
        {
            processQ3();
        }
        else if (!empty_queue(&q4))
        {
            processQ4();
        }
        else if (runningProcessCount <= 0 && count == 0 && blockedCount == 0)
        {
            break;
        }
        else
        {
            printf("RUN: Process <<null>> executing at time %d.\n", currentTime);
            currentTime++;
            nullProcessTime++;
        }
    }
    printf("Scheduler shutdown at time %d.\n", currentTime);
    printf("Total CPU usage for all processes scheduled: \n");
    printf("Process <<null>>: %d time units.\n", nullProcessTime);

    rewind_queue(&finishedProcesses);
    while (!end_of_queue(&finishedProcesses))
    {
        Process p;
        peek_at_current(&finishedProcesses, &p);
        printf("Process %d:\t %d time units.\n", p.pid, p.cpuTimeUsed);
        next_element(&finishedProcesses);
    }
  
    return 0;
}