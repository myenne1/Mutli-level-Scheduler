#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prioque.h"

typedef struct _Process
{
    int time;
    int pid;
    int run;
    int io;
    int repeat;
} Process;

int main(int argc, char *argv[])
{
    Process processes[11];
    Queue q1, q2, q3, q4;
    Queue readyQueue1, readyQueue2, readyQueue3, readyQueue4;

    printf("Queue entries one per line: \n");
    int count = 0;
    while (scanf("%d %d %d %d %d", &processes[count].time,
                 &processes[count].pid,
                 &processes[count].run,
                 &processes[count].io,
                 &processes[count].repeat) == 5)
    {
        count++;
    }

    int currentTime = 0;

    // Initialize queues
    init_queue(&readyQueue1, sizeof(Queue), TRUE, NULL, TRUE); // readyQueue1 = highest priority
    init_queue(&readyQueue2, sizeof(Queue), TRUE, NULL, TRUE);
    init_queue(&readyQueue3, sizeof(Queue), TRUE, NULL, TRUE);
    init_queue(&readyQueue4, sizeof(Queue), TRUE, NULL, TRUE);

    init_queue(&q1, sizeof(Queue), TRUE, NULL, FALSE); // q1 = highest priority
    init_queue(&q2, sizeof(Queue), TRUE, NULL, FALSE);
    init_queue(&q3, sizeof(Queue), TRUE, NULL, FALSE);
    init_queue(&q4, sizeof(Queue), TRUE, NULL, FALSE);

    // Start simulation
    while (count != 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (processes[i].time == currentTime)
            {
                printf("PID: %d Arrival Time: %d\n", processes[i].pid, currentTime);
                add_to_queue(&readyQueue1, &processes[i], processes[i].pid);
                printf("CREATE: Process %d entered the ready queue at time %d\n", processes[i].pid, currentTime);
            }
        }

        currentTime++;
    }
}