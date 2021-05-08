#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simcpu.h"
#include <stdbool.h>

int TotalThreadNumber = 0;
int size = 0;
int rrSize = 0;

int main(int argc, char *argv[])
{
    int dMode = 0;
    int vMode = 0;
    int rMode = 0;
    int quant = 0;

    for (int flags = 0; flags < argc; flags++)
    {
        // check for detailed mode 
        if (strcmp(argv[flags], "-d") == 0)
        {
            dMode = 1;
        }
        // check for verbose mode
        if (strcmp(argv[flags], "-v") == 0)
        {
            vMode = 1;
        }
        // check for quantom time 
        if (strcmp(argv[flags], "-r") == 0)
        {
            rMode = 1;
            quant = atoi(argv[flags + 1]);
            if (quant == 0)
            {
                printf("Time Quantom Cannot be 0\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    if (rMode == 1)
    {
        // run round robin
        RR(dMode, vMode, quant);
    }
    else
    {
        // run first come first serve 
        FCFS(dMode, vMode);
    }
}

/**
 * extract the minimum values from the priority queue and sort it by arrival time 
*/
Thread **InsertArray(Thread **information, int *size, int value)
{
    Thread **info = malloc(sizeof(Thread) * MAXSIZE);
    Thread *thread = removeMin(information, size);

    int index = 0;
    while (thread != NULL)
    {
        memcpy(&info[index], &thread, sizeof(Thread));
        thread = removeMin(information, size);
        index++;
    }
    return info;
}
/**
 * insert into ready Queue 
*/
void readyQueue(Thread ***thread, Thread ***ready, int *readySize, int *threadSize, int *waitTime, int vMode)
{
    // get the minimum thread 
    Thread *minThread = getMin(*thread);
    if (minThread != NULL)
    {
        int arrivalTime = minThread->arrivalTime;
        // check if arrival time is equal to the current time
        if (arrivalTime == *waitTime)
        {
            //extract minimum thread at the top of the priority queue 
            removeMin(*thread, threadSize);
            
            // prints verbose mode if specified 
            if (vMode == 1)
            {
                printf("At time %d: Thread %d of Process %d moves from new to ready\n", minThread->arrivalTime, minThread->threadNumber, minThread->processNumber);
            }
            // insert the minimum thread into the ready Queue 
            insert(*ready, minThread, readySize);

            while (minThread->arrivalTime == *waitTime && *threadSize != 0)
            {

                minThread = getMin(*thread);
                if (minThread->arrivalTime == *waitTime)
                {
                    // prints verbose mode if specified 
                    if (vMode == 1)
                    {
                        printf("At time %d: Thread %d of Process %d moves from new to ready\n", minThread->arrivalTime, minThread->threadNumber, minThread->processNumber);
                    }
                    //extract minimum thread at the top of the priority queue 
                    minThread = removeMin(*thread, threadSize);
                    // insert the minimum thread into the ready Queue 
                    insert(*ready, minThread, readySize);
                }
            }
        }
    }
}
// read file and initialize Priority Queues 
void initializeSchedule(Process **process, Thread ***thread, Thread ***finish, Thread ***ready, Thread ***IO, Thread **CPU)
{
    *process = malloc(sizeof(Process));
    *thread = readFile(*process);

    *CPU = malloc(sizeof(Thread));
    *ready = malloc(sizeof(Thread *) * MAXSIZE);
    *finish = malloc(sizeof(Thread *) * MAXSIZE);
    *IO = malloc(sizeof(Thread *) * MAXSIZE);
}

void getcontextSwitch(Process **process, Thread **CPU, Thread ***ready, int *readySize, int *waitTime, int *switchCounter, int *isCPUEmpty, int *switchTime, int vMode)
{
    // extract minimum from top of ready Queue
    *CPU = removeMin(*ready, readySize);
    (*CPU)->timeEntered = *waitTime;

    Thread *minm = getMin(*ready);
    if (minm->processNumber != (*CPU)->processNumber)
    {
        // if we switch from process number to a different process number 
        *switchTime = (*process)->processSwitch;
    }
    else
    {
        // if both processes have same process number 
        *switchTime = (*process)->threadSwitch;
    }
    // set the time CPU enters 
    (*CPU)->timeEntered = *waitTime;

    // prints Verbose mode if specified
    if (vMode == 1)
    {
        printf("At time %d: Thread %d of Process %d moves from ready to running\n", (*CPU)->timeEntered, (*CPU)->threadNumber, (*CPU)->processNumber);
    }
    *switchCounter = 1;
    *isCPUEmpty = 1;
}
void FCFS(int dMode, int vMode)
{
    Process *process;
    Thread *CPU;
    Thread **finish, **ready, **IO, **thread;

    // parse the file and intitialize priority queues
    initializeSchedule(&process, &thread, &ready, &finish, &IO, &CPU);

    int i = 0;
    int finishLength = 0;
    int switchTime = process->processSwitch, waitTime = 0;
    int cpuCounter = 0, switchCounter = 1;
    int readySize = 0, finishSize = 0, IOSize = 0;
    int isCPUEmpty = 0, isSwitchTaken = 1;
    int totalSwitches = 0, TotalCpuTime = 0;

    while (finishLength != TotalThreadNumber)
    {
        // insert into ready queue
        readyQueue(&thread, &ready, &readySize, &size, &waitTime, vMode);

        if (isCPUEmpty == 0)
        {

            if (isSwitchTaken == 1)
            {
                // extract minimum from top of Ready priority Queue into CPU 
                CPU = removeMin(ready, &readySize);
                CPU->timeEntered = waitTime;
                isSwitchTaken = 0;

                // print verbose mode if specified 
                if (vMode == 1)
                {
                    printf("At time %d: Thread %d of Process %d moves from ready to running\n", CPU->timeEntered, CPU->threadNumber, CPU->processNumber);
                }
                isCPUEmpty = 1;
                isSwitchTaken = 0;
            }
            else
            {
                if (switchCounter == switchTime)
                {
                    getcontextSwitch(&process, &CPU, &ready, &readySize, &waitTime, &switchCounter, &isCPUEmpty, &switchTime, vMode);
                }
                else
                {
                    totalSwitches++;
                    switchCounter++;
                }
            }
        }

        if (isCPUEmpty == 1)
        {
            Burst *burst = CPU->burst[CPU->burstIndex];

            if (burst->cputime == cpuCounter)
            {

                isCPUEmpty = 0;
                cpuCounter = 0;

                if (burst->burstNum == CPU->numberOfCPU)
                {
                    // do the calculations 
                    CPU->finishTime = waitTime;
                    CPU->turnaroundTime = CPU->finishTime - CPU->arrivalTimeReady;
                    CPU->serviceTime += burst->cputime;

                    // verbose mode print if specified 
                    if (vMode == 1)
                    {
                        printf("At time %d: Thread %d of Process %d moves from ready to finished\n", CPU->finishTime, CPU->threadNumber, CPU->processNumber);
                    }

                    // insert into finish Priority Queue from CPU
                    CPU->ready = 0;
                    insert(finish, CPU, &finishSize);
                    finishLength++;
                }
                else
                {
                    // get service time
                    CPU->serviceTime += burst->cputime;

                    // verbose mode print if specified 
                    if (vMode == 1)
                    {
                        printf("At time %d: Thread %d of Process %d moves from ready to blocked\n", waitTime, CPU->threadNumber, CPU->processNumber);
                    }
                    // insert into IO Priority Queue from CPU
                    insert(IO, CPU, &IOSize);
                }
            }
            else
            {
                TotalCpuTime++;
                cpuCounter++;
            }
        }

        if (IOSize != 0)
        {
            // insert from IO Queue into thread
            IOQueue(&IO, &thread, &IOSize, &waitTime, &size, vMode);
        }

        waitTime++;
    }

    // insert into finish Queue 
    int k = finishSize;
    Thread **arr = InsertArray(finish, &finishSize, k - 1);
    double avgTaTime = 0;
    double util = (TotalCpuTime / (waitTime - 1)) * 100;

    // caclalate the cpu utilization
    i = 0;
    while (i < k)
    {
        util += arr[i]->serviceTime;
        i++;
    }
    // calculate the average turn around time
    avgTaTime = getAvgTurnaround(process, arr);
    util = util / (waitTime - 1) * 100;

    // print default mode details 
    defaultMode(waitTime, avgTaTime, util);

    // detailed mode printing if specified 
    if (dMode == 1)
    {
        detailedMode(process, arr);
    }
}
void IOQueue(Thread ***IO, Thread ***thread, int *IOSize, int *waitTime, int *threadSize, int vMode)
{
    int index = *IOSize;
    int i = 0;
    Thread **copy1 = malloc(sizeof(Thread *) * MAXSIZE);
    int k = 0;

    // insert into IO 
    Thread **devices = InsertArray(*IO, IOSize, index);

    while (i < index)
    {

        Thread *device = devices[i];

        if (device->burstIndex != device->numberOfCPU)
        {
            Burst *burst = device->burst[device->burstIndex];
            burst->IOtime = burst->IOtime - 1;

            if (burst->IOtime == 0)
            {
                device->arrivalTime = *waitTime + 1;
                // print verbose mode if specified 
                if (vMode == 1)
                {
                    printf("At time %d: Thread %d of Process %d moves from blocked to unblocked\n", device->arrivalTime, device->threadNumber, device->processNumber);
                }

                // insert from IO into thread
                device->burstIndex++;
                // insert into thread Queue from IO
                insert(*thread, device, threadSize);
            }
            else
            {
                copy1[k] = malloc(sizeof(Thread));
                memcpy(&copy1[k], &device, sizeof(Thread));
                k++;
            }
        }

        i++;
    }
    free(devices);
    *IOSize = moveIt(IO, copy1, k);
}
int moveIt(Thread ***information, Thread **InsertArray, int value)
{
    int size = 0;
    for (int index = 0; index < value; index++)
    {
        insert(*information, InsertArray[index], &size);
    }
    return size;
}
// print default mode 
void defaultMode(int waitTime, double avgTaTime, double util)
{
    printf("\nTotal time required = %d units\n", waitTime - 1);
    printf("Average Turnaround Time: %.1f\n", avgTaTime);
    printf("CPU Utilization = %.1f%%\n", util);
}

void RR(int dMode, int vMode, int quant)
{

    Process *process;
    Thread *CPU;
    Thread **finish, **ready, **IO, **thread;

    // parse the file and intitialize priority queues
    initializeSchedule(&process, &thread, &ready, &finish, &IO, &CPU);

    int i = 0;
    int finishLength = 0;
    int switchTime = process->processSwitch, waitTime = 0;
    int cpuCounter = 0, switchCounter = 1;
    int readySize = 0, finishSize = 0, IOSize = 0;
    int isCPUEmpty = 0, isSwitchTaken = 1;
    int totalSwitches = 0, totalCPUTime = 0;

    while (finishLength != TotalThreadNumber)
    {
        // insert into ready queue
        readyQueue(&thread, &ready, &readySize, &rrSize, &waitTime, vMode);

        if (isCPUEmpty == 0)
        {

            if (isSwitchTaken == 1)
            {
                // extract minimum from top of Ready priority Queue into CPU 
                CPU = removeMin(ready, &readySize);
                CPU->timeEntered = waitTime;
                isSwitchTaken = 0;
                // print verbose mode if specified 
                if (vMode == 1)
                {
                    printf("At time %d: Thread %d of Process %d moves from ready to running\n", CPU->timeEntered, CPU->threadNumber, CPU->processNumber);
                }
                isCPUEmpty = 1;
                isSwitchTaken = 0;
            }
            else
            {

                if (switchCounter == switchTime)
                {

                    if (readySize > 0)
                    {
                        // extract minimum from top of Ready priority Queue into CPU 
                        CPU = removeMin(ready, &readySize);
                        CPU->timeEntered = waitTime;

                        Thread *minm = getMin(ready);

                        if (readySize == 0)
                        {
                            switchTime = 1;
                            switchCounter = 1;
                        }
                        else
                        {

                            if (minm->processNumber != CPU->processNumber)
                            {
                            // if we switch from process number to a different process number 
                                switchTime = process->processSwitch;
                            }
                            else
                            {
                             // if both process have same process number
                                switchTime = process->threadSwitch;
                            }
                        }
                        // print verbose mode if specified 
                        if (vMode == 1)
                        {
                            printf("At time %d: Thread %d of Process %d moves from ready to running\n", CPU->timeEntered, CPU->threadNumber, CPU->processNumber);
                        }
                        switchCounter = 1;
                        isCPUEmpty = 1;
                    }
                }
                else
                {
                    totalSwitches++;
                    switchCounter++;
                }
            }
        }

        if (isCPUEmpty == 1)
        {
            Burst *burst = CPU->burst[CPU->burstIndex];

            if (burst->cputimeRobin <= quant)
            {

                if (burst->cputimeRobin == cpuCounter)
                {
                    isCPUEmpty = 0;
                    cpuCounter = 0;
                    if (burst->burstNum == CPU->numberOfCPU)
                    {
                         // do the calculations 
                        CPU->finishTime = waitTime;
                        CPU->serviceTime += burst->cputime;
                        CPU->turnaroundTime = CPU->finishTime - CPU->arrivalTimeReady;

                        // verbose mode print if specified 
                        if (vMode == 1)
                        {
                            printf("At time %d: Thread %d of Process %d moves from ready to finished\n", CPU->finishTime, CPU->threadNumber, CPU->processNumber);
                        }
                        CPU->ready = 0;
                        // insert into finish Priority Queue from CPU
                        insert(finish, CPU, &finishSize);
                        finishLength++;
                    }
                    else
                    {
                        // get service time
                        CPU->serviceTime += burst->cputime;

                        // verbose mode print if specified 
                        if (vMode == 1)
                        {
                            printf("At time %d: Thread %d of Process %d moves from ready to blocked\n", waitTime, CPU->threadNumber, CPU->processNumber);
                        }

                        // insert to IO Queue    
                        insert(IO, CPU, &IOSize);
                    }
                }
                else
                {
                    
                    totalCPUTime++;
                    cpuCounter++;
                }
            }
            else if (burst->cputimeRobin > quant)
            {

                if (cpuCounter == quant)
                {
                    isCPUEmpty = 0;
                    cpuCounter = 0;

                    burst->cputimeRobin = burst->cputimeRobin - quant;
                    CPU->arrivalTime = waitTime + 1;
                    // insert into ready Priority Queue from CPU
                    insert(ready, CPU, &readySize);
                }
                else
                {
                    totalCPUTime++;
                    cpuCounter++;
                }
            }
        }

        if (IOSize != 0)
        {
            // insert from IO queue into thread
            IOQueue(&IO, &thread, &IOSize, &waitTime, &rrSize, vMode);
        }

        waitTime++;
    }

    // insert into finish Queue 
    int k = finishSize;
    Thread **arr = InsertArray(finish, &finishSize, k);
    double avgTaTime = 0;

    // caclalate the cpu utilization
    double util = (totalCPUTime / (waitTime - 1)) * 100;
    i = 0;
    while (i < k)
    {
        util += arr[i]->serviceTime;
        i++;
    }
    util = util / (waitTime - 1) * 100;
    // calculate the average turn around time
    avgTaTime = getAvgTurnaround(process, arr);
    // print default mode details 
    defaultMode(waitTime, avgTaTime, util);

    // print detailed mode if specified  
    if (dMode == 1)
    {
        detailedMode(process, arr);
    }
}
// print detailed mode 
void detailedMode(Process *process, Thread **arr)
{
    int currentThread = 1;
    for (int i = 1; i <= process->numOfProcesses; i++)
    {
        currentThread = 1;
        for (int k = 0; k < TotalThreadNumber; k++)
        {
            Thread *device = arr[k];
            if (device->processNumber == i && device->threadNumber == currentThread)
            {
                printf("Thread %d  Process %d:\n", device->threadNumber, device->processNumber);
                printf("\tarrival time: %d units\n\tservice time: %d units, I/O time: %d units, turnaround time: %d units, finish time: %d units\n", device->arrivalTimeReady, device->serviceTime, device->totalIOTime, device->turnaroundTime, device->finishTime);
                currentThread++;
                k = 0;
            }
        }
    }
}
double getAvgTurnaround(Process *process, Thread **arr)
{
    int i = 1;
    double avgTaTime = 0;
    while (i <= process->numOfProcesses)
    {
        int minimum = -1;
        int maximum = -1;
        int index = 0;
        while (index < TotalThreadNumber)
        {
            Thread *thread2 = arr[index];
            if (thread2->threadNumber == 1 && thread2->processNumber == i)
            {
                // min arrival time (arrival time of first thread of the proces)
                minimum = thread2->arrivalTimeReady; 

                //max finish time of the process
                maximum = thread2->finishTime;  
            }
            if (maximum != -1 && thread2->processNumber == i)
            {
                // compare the current max with finish time and set it to new max 
                maximum = MAX(thread2->finishTime, maximum);
            }
            index++;
        }
        if (maximum != -1 && minimum != -1)
        {
            // average turn around = average turnaround + max finish time of the process - min arrival time (arrival time of first thread of the proces)
            avgTaTime += maximum - minimum;
        }
        i++;
    }
    avgTaTime = (avgTaTime / process->numOfProcesses);
    return avgTaTime;
}
// parse the file 
Thread **readFile(Process *process)
{
    // read the process from the file and store it into the struct process
    fscanf(stdin, "%d %d %d", &process->numOfProcesses, &process->threadSwitch, &process->processSwitch);

    Thread **thread = malloc(sizeof(Thread *) * MAXSIZE);

    for (int i = 0; i < process->numOfProcesses; i++)
    { 

        int processNumber = 0;
        int numOfThreads = 0;
        fscanf(stdin, "%d %d", &processNumber, &numOfThreads);

        for (int j = 0; j < numOfThreads; j++)
        { 

            int threadNumber = 0;
            int arrivalTime = 0;
            int numberOfCPU = 0;

            // read the process from the file an store it into the struct aThread
            fscanf(stdin, "%d %d %d", &threadNumber, &arrivalTime, &numberOfCPU);

            Thread *aThread = malloc(sizeof(Thread));

            aThread->processNumber = processNumber;
            aThread->numOfThreads = numOfThreads;

            aThread->threadNumber = threadNumber;
            aThread->arrivalTime = arrivalTime;
            aThread->arrivalTimeReady = arrivalTime;
            aThread->numberOfCPU = numberOfCPU;
            aThread->processNumber = processNumber;
            aThread->burst = malloc(sizeof(Burst *) * numberOfCPU);
            aThread->burstIndex = 0;
            aThread->ready = 1;

            aThread->serviceTime = 0;
            aThread->turnaroundTime = 0;
            aThread->finishTime = 0;
            aThread->timeEntered = 0;

            for (int k = 0; k < numberOfCPU; k++)
            {
                int burstNum = 0;
                int IOtime = 0;
                int cputime = 0;

                // read the burst from the struct burst

                fscanf(stdin, "%d", &burstNum);

                if (aThread->numberOfCPU != burstNum)
                {
                    fscanf(stdin, "%d %d", &cputime, &IOtime);
                }
                else
                {
                    fscanf(stdin, "%d", &cputime);
                }

                aThread->burst[k] = malloc(sizeof(Burst));
                aThread->burst[k]->burstNum = burstNum;
                aThread->burst[k]->cputime = cputime;
                aThread->burst[k]->IOtime = IOtime;
                aThread->burst[k]->cputimeRobin = cputime;
                aThread->totalIOTime += IOtime;
            }

            // insert into heap (from aThread into thread)
            insert(thread, aThread, &size);
            rrSize++;
            TotalThreadNumber++;
        }
    }

    return thread;
}