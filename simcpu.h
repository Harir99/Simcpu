#ifndef SIMCPU_H
#define SIMCPU_H

#define MAXSIZE 2000

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct Process {
    int numOfProcesses;
    int threadSwitch;
    int processSwitch;
} Process;

typedef struct Burst {
    int IOtime;
    int cputime;
    int burstNum;
    int cputimeRobin;
} Burst;

typedef struct Thread {
    int numOfProcesses;
    int processNumber;
    int numOfThreads;
    int threadNumber;
    int arrivalTime;
    int numberOfCPU;
    int totalIOTime;
    int totalCPUTime;
    int burstTime;
    int ready;
    int burstIndex;
    int timeEntered;
    int finishTime;
    int serviceTime;
    int turnaroundTime;
    int arrivalTimeReady;
    Burst** burst;
} Thread;

void FCFS(int dMode, int vMode);
void RR(int dMode,int vMode, int quant);
Thread **InsertArray(Thread **information, int *size, int value);
Thread **readFile (Process *process);
int moveIt(Thread*** information,Thread ** InsertArray,int value);
int rightChildIndex(int index);
int leftChildIndex(int index);
int parentIndex(int index);
Thread **parseFile(Process *proc);                        
void swap(Thread *a, Thread *b);
int get_node(Thread *thread[], int index, int flag);
void bubbleDown(Thread *thread[], int index, int *heap_size);
void heapify(Thread *thread[], int *heap_size);
Thread *getMin(Thread *thread[]);
Thread *removeMin(Thread *thread[], int *heap_size);
void decrease_key(Thread *thread[], int index, Thread *key);
void insert(Thread *thread[], Thread *key, int *heap_size);
void detailedMode(Process *process, Thread **arr);
double getAvgTurnaround(Process *process, Thread **arr);
void readyQueue(Thread ***thread, Thread ***ready, int *readySize, int *threadSize, int *waitTime, int vMode);
void defaultMode(int waitTime, double avgTaTime, double util);
void IOQueue(Thread ***IO, Thread ***thread, int *IOSize, int *waitTime,int*threadSize, int vMode);
void initializeSchedule(Process** process, Thread*** thread,Thread*** ready,Thread*** finish,Thread*** IO,Thread** CPU);
void getcontextSwitch(Process **process, Thread **CPU, Thread ***ready, int *readySize, int *waitTime, int *switchCounter, int *isCPUEmpty, int *switchTime, int vMode);

#endif //  SIMCPU_H
