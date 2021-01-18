#include "headers.h"
#include "queue.c"

int shmid, INITIAL_MSG_Q_ID, sem1, schedulerPID, sem2;
bool start = false;
void clearResources(int);
void sendNewProcess(int shmid, processData processToBeSent)
{
    processData* shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void*)-1)
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
void sendSchedulingType(int INITIAL_MSG_Q_ID, schedulingAlgorithm s, int quantum, int numProcesses)
{
    //Construct the message 
    msgbuff message;
    message.mtype = 50; 

    message.initialSchedulingData.algo = s;
    message.initialSchedulingData.quantum = quantum;
    message.initialSchedulingData.numProcesses = numProcesses;

    //Send the message
    int send_val = msgsnd(INITIAL_MSG_Q_ID, &message, sizeof(message.initialSchedulingData), !IPC_NOWAIT);

    //Handle unexpected errors by notifying the user
    if (send_val == -1)
        perror("Errror in sending the initial data to the scheduler.");
}
void canStartSending(int signum)
{
    start = true;
    printf("START \n");
    signal(SIGHUP,canStartSending);
}
int main()
{
    signal(SIGINT, clearResources);
    signal(SIGHUP,canStartSending);
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
    int clockPID = fork(), stat_loc_sched, stat_loc;
    
    if (clockPID == -1)
  	    perror("error in fork");	
    else if (clockPID == 0)
    {
        printf("clock \n");
	   execl("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/clk.o", "clk.o", NULL);
    }
    schedulerPID = fork();
    if (schedulerPID == -1)
    perror("error in fork");
    else if (schedulerPID == 0)
    {
       execl("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/scheduler.o", "scheduler.o", NULL); 
    }


    //Create a message Queue for sending the initial data(scheduling type, parameters,number of processes)
    INITIAL_MSG_Q_ID = msgget(55, 0666 | IPC_CREAT); 

    //Handle unexpected errors by notifying the user and shutting down
    if (INITIAL_MSG_Q_ID == -1)
    {
        perror("Error in create");
        exit(-1);
    }

    key_t shm_ID;
	shm_ID = ftok("keyfile",150);
    shmid = shmget(77, sizeof(processData), IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("Error in create gen");
        printf(" 1: %d \n", shmid);
        exit(-1);
    }
    else
    {
        printf("Shared memory ID p gen: %d \n", shmid);
    }
     
    union Semun semun; 
    sem1 = semget(SEM_ID_PG_TO_SCH, 1, 0666 | IPC_CREAT);
    sem2 = semget(SEM_ID_PG_TO_SCH_FIN, 1, 0666 | IPC_CREAT);

    if (sem1 == -1 || sem2 == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = 1; /* initial value of the semaphore, Binary semaphore */
    
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    semun.val = 0;
    if (semctl(sem2, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    struct Queue* processQueue = createQueue();
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
        char * token5 = strtok(NULL, "\t");
        p->memorySize = atoi(token5);
        p->waitingTime = 0;
        p->lastBlockingTime = 0;
        p->startTime = 0;
        p->idleTime = 0;
        inserted = enqueueQueue(processQueue, p);
        if (!inserted)
            printf("Reached max number of processes!"); 
    }
    
    // 6. Send the information to the scheduler at the appropriate time.
    bool sentAllProcesses = false;
    int counter = 0;
    int now;
    int sizeOfQueue = processQueue->size;
    sendSchedulingType(INITIAL_MSG_Q_ID, chosenAlgorithm, quantum, sizeOfQueue);
    processData * processToBeSent;
    while(start == false);
    for (int i = 0; i < sizeOfQueue; i++)
    {
        while (getClk() < processQueue->front->data->arrivalTime);
        printf("Now send process number %d at time %d \n", i,getClk());
        //if we reach this point, then we got to/passed the arrival time of the process
        processToBeSent = dequeueQueue(processQueue);
        if (processToBeSent != NULL)
        {
            down(sem1);
            sendNewProcess(shmid, *processToBeSent);
            kill(schedulerPID,SIGUSR1);
        }
    }
    if (sizeOfQueue != 0)
    {
        printf("show down now \n");
        down(sem2);
    }
    kill(schedulerPID,SIGUSR2);
    printf("done in p gen\n");
    // 7. Clear clock resources
    schedulerPID = wait(&stat_loc_sched);
    destroyClk(true);
    
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    msgctl(INITIAL_MSG_Q_ID, IPC_RMID, (struct msqid_ds *)0); 
    semctl(sem1, 1, IPC_RMID);
    semctl(sem2, 1, IPC_RMID);
    kill(schedulerPID, SIGINT);
}
