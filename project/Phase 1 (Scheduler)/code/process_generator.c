#include "headers.h"
#include "queue.c"


void clearResources(int);
void sendNewProcess(int shmid, processData processToBeSent)
{
    processData* shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        *shmaddr = processToBeSent;
    }
    shmdt(shmaddr);
}
void sendSchedulingType(int shmid2, schedulingAlgorithm s, int quantum, int numProcesses)
{
    schedulingType* shmaddr = shmat(shmid2, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        schedulingType temp;
        temp.algo = s;
        temp.quantum = quantum;
        temp.numProcesses = numProcesses;
        *(shmaddr) = temp;
    } 
    shmdt(shmaddr);
}
int main()
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE* inputFile = fopen("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/processes.txt","r");
    if (inputFile == NULL ) 
    {   
        printf("Error! Could not open processes file\n"); 
        exit(-1); 
    } 
    int endMark = getc(inputFile);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int chosenAlgorithm = -1, quantum = -1;
    printf("Please choose the desired scheduling algorithm: \n");
    printf("1: Highest Priority First (HPF) \n");
    printf("2: Shortest Remaining Time Next (SRTN) \n");
    printf("3: Round Robin (RR) \n");
    scanf("%d", &chosenAlgorithm);
    if (chosenAlgorithm == 3)
    {
        printf("Please enter the desired quantum value: \n");
        scanf("%d", &quantum);  
    }
    // 3. Initiate and create the scheduler and clock processes.
    int clockPID = fork(), schedulerPID = fork(), stat_loc_sched, stat_loc;
    
    if (clockPID == -1)
  	    perror("error in fork");	
    else if (clockPID == 0)
    {
        printf("clock \n");
	   execl("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/clk.o", "clk.o", NULL);
    }
    if (schedulerPID == -1)
    perror("error in fork");
    else if (schedulerPID == 0)
    {
       execl("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/scheduler.o", "scheduler.o", NULL); 
    }
    key_t shm_ID, shm_ID2;
	shm_ID = ftok("keyfile",150);
    shm_ID = ftok("keyfile",200);
    int shmid = shmget(100, sizeof(processData), IPC_CREAT | 0644);
    int shmid2 = shmget(200, sizeof(schedulingType*), IPC_CREAT | 0644);
    if (shmid == -1 || shmid2 == -1)
    {
        perror("Error in create gen");
        printf(" 1: %d , 2: %d \n", shmid, shmid2);
        exit(-1);
    }
    else
    {
        printf("Shared memory ID p gen: %d \n", shmid);
        printf("Shared memory ID (alg type)p gen: %d \n", shmid2);
    }
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    struct Queue* processQueue = createQueue();
    //printf("1-Size of queue: %d \n", processQueue->size);
    bool inserted = false;
    processData *p;
    char * str_operation;
    char* tab = "/t";
    int arr[4];
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, inputFile)) != -1) {
        processData *p = ((processData*) malloc(sizeof(processData)));
        char * token = strtok(line, "\t");
        if (strspn(token, "0123456789") != strlen(token))
            continue;
        p->id = atoi(token);
        char * token2 = strtok(NULL, "\t");
        p->arrivalTime = atoi(token2);
        char * token3 = strtok(NULL, "\t");
        p->runningTime = atoi(token3);
        p->remainingTime = atoi(token3);
        char * token4 = strtok(NULL, "\t");
        p->priority = atoi(token4);
        p->waitingTime = 0;
        p->lastBlockingTime = 0;
        p->memorySize = 0;
        //printf("%d %d %d %d \n", p->id, p->arrivalTime, p->runningTime, p->priority);
        inserted = enqueueQueue(processQueue, p);
        if (!inserted)
            printf("Reached max number of processes!"); 
    }
    // 6. Send the information to the scheduler at the appropriate time.
    bool sentAllProcesses = false;
    int counter = 0;
    int now;
    int sizeOfQueue = processQueue->size;
    sendSchedulingType(shmid2, chosenAlgorithm, quantum, sizeOfQueue);
    processData * processToBeSent;
    for (int i = 0; i < sizeOfQueue; i++)
    {
        now = getClk();
        while (now < processQueue->front->data->arrivalTime)
        {
            now = getClk();
        }
        //printf("Now send process number %d at time %d \n", i,getClk());
        //if we reach this point, then we got to/passed the arrival time of the process
        processToBeSent = dequeueQueue(processQueue);
        if (processToBeSent != NULL)
        {
            sendNewProcess(shmid, *processToBeSent);
            kill(schedulerPID,SIGUSR1);
            printf("Data to be sent %d %d %d %d \n", processToBeSent->id, processToBeSent->arrivalTime, processToBeSent->runningTime, processToBeSent->priority);
        }
        //printf("-------------------- \n");
    }
    sleep(5);
    kill(schedulerPID,SIGUSR2);
    printf("done in p gen\n");
    // 7. Clear clock resources
    destroyClk(true);
    clockPID = wait(&stat_loc);
    schedulerPID = wait(&stat_loc_sched);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
