#include "priorityQueue.c"
#include "hash.c"
#include "queue.c"
#include "logger.c"
#include "memory.c"
#define PROCESSES_NUMBER 5

int shmid;
struct Hash* PCB;
struct PriorityQueue* queueHPF;
struct PriorityQueue* queueSTRN;
struct Queue* queueRR;
schedulingAlgorithm type;
bool finish = false;

void insertProcessPriorityQueue(struct PriorityQueue* queue,processData* process,int clock)
{
    if(process != NULL && process->arrivalTime == clock)
    {
        //printf("\nat t=%d: Process with ID %d has arrived!\n",clock,process->id);
        enqueuePriorityQueue(queue,process);    
    }
}


void insertProcessQueue(struct Queue* queue,processData* process,int clock)
{
    if(process != NULL && process->arrivalTime == clock)
    {
        //printf("\nat t=%d: Process with ID %d has arrived!\n",clock,process->id);
        enqueueQueue(queue,process);    
    }
}

// Highest  First
bool doHPF(int clock,struct PriorityQueue* queue,struct Hash* PCB)
{
    // Initializing
    static int processessCounter = 0;
    static processData* process = NULL;

    if(!isEmptyHash(PCB)){
        // Get current process & process info
        if(process == NULL) process = dequeuePriorityQueue(queue);
        if(process != NULL)
        {
            // Calculate waiting time for this process
            process->waitingTime += clock - process->lastBlockingTime;
            if(process->waitingTime)
                process->waitingTime = process->waitingTime;
            // Updateing time variables
            process->lastBlockingTime = clock+1;
            process->remainingTime --;

            if(process->remainingTime == 0)
            {
                //printf("\n--> Process %d with priority %d has finished in t=%d after waiting %ds <--\n",process->id,process->priority,clock,process->waitingTime);
                hashRemove(PCB,process->id);
                processessCounter++;
                process = NULL;
                if(processessCounter == PROCESSES_NUMBER)
                    return true;

            }
        }
        
    }

    return false;
}

// Shortest Time Ramining Next
bool doSTRN(int clock,struct PriorityQueue* queue,struct Hash* PCB){
    // Initializing
    static processData* prev = NULL; 
    static int processessCounter = 0;

    if(!isEmptyPriorityQueue(queue)){
        // Get current process & process info
        processData* process = frontPriorityQueue(queue);

        // Checking on context switching (DEBUGGING PURPOSES)
        if( process != prev)
        {
            //if(prev != NULL) printf("\nAt t=%d: Process %d with RT=%d has swtiched the context to process %d with RT=%d\n",clock,prev->id,prev->remainingTime,process->id,process->remainingTime);
            //else printf("\nAt t=%d: Switching the context to process %d with RT=%d\n",clock,process->id,process->remainingTime);
            prev= process;
        }
        // Calculate waiting time for this process
        process->waitingTime += clock - process->lastBlockingTime;

        // Move remaining time
        process->remainingTime --;
        if(process->remainingTime == 0)
        {
            //printf("\n--> Process %d with running time %d has finished in t=%d after waiting %ds <--\n",process->id,process->runningTime,clock,process->waitingTime);
            dequeuePriorityQueue(queue);
            hashRemove(PCB,process->id);
            processessCounter++;
            if(processessCounter == PROCESSES_NUMBER)
                return true;

            prev = NULL;
        }
        process->lastBlockingTime = clock+1;
    }

    return false;
}
  
// Round Roben
bool doRR(int clock,struct Queue* queue,struct Hash* PCB, unsigned int quantum)
{
    // Initializing
    static int processessCounter = 0;
    static int quantumCounter = 0;
    static processData* process = NULL;
    //Check if new process comes 
    ///TODO:(SHOULD BE REPLACED)
    insertProcessQueue(queue,hashFind(PCB,1),clock);
    insertProcessQueue(queue,hashFind(PCB,2),clock);
    insertProcessQueue(queue,hashFind(PCB,3),clock);
    insertProcessQueue(queue,hashFind(PCB,4),clock);
    insertProcessQueue(queue,hashFind(PCB,5),clock); 

    if(!isEmptyHash(PCB)){
        // Get current process & process info
        if(process == NULL) 
        {
            process = dequeueQueue(queue);
            //if(process != NULL) 
                //printf("\nat t=%d: The context is switched to process %d\n",clock,process->id);
        }
        if(process != NULL)
        {
            // Calculate waiting time for this process
            process->waitingTime += clock - process->lastBlockingTime;
            if(process->waitingTime)
                process->waitingTime = process->waitingTime;
            // Updateing time variables
            process->lastBlockingTime = clock+1;
            process->remainingTime --;

            if(process->remainingTime == 0)
            {
                //printf("\n--> Process %d has finished in t=%d after waiting %ds <--\n",process->id,clock,process->waitingTime);
                hashRemove(PCB,process->id);
                processessCounter++;
                process = NULL;
                if(processessCounter == PROCESSES_NUMBER)
                    return true;
                return false;
            }

            // Increase quantum counter then check if the process finished its quantum
            quantumCounter++; 
            if(quantumCounter == quantum)
            {
                enqueueQueue(queue,process);
                process = NULL;
                quantumCounter = 0;
            }
        }
        
    }

    return false;
}
processData receiveNewProcess(int shmid)
{
    processData *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    return *shmaddr;
}
schedulingType receiveSchedulingType(int INITIAL_MSG_Q_ID)
{
    msgbuff messageReceived;

    //recieve only the messages of 50 message type
    int rec_val = msgrcv(INITIAL_MSG_Q_ID, &messageReceived, sizeof(messageReceived.initialSchedulingData), 50, !IPC_NOWAIT);

    //Handle unexpected errors by notifying the user
    if (rec_val == -1)
        perror("Error in receiving the initial data from the process generator.");

    return messageReceived.initialSchedulingData;
}
void newProcessArrived(int signum)
{
    printf("Entered sig handler in scheduler \n");
    processData newProcess = receiveNewProcess(shmid);
    processData* temp = (processData*)malloc(sizeof(processData));
    temp->id = newProcess.id;
    temp->arrivalTime = newProcess.arrivalTime;
    temp->runningTime = newProcess.runningTime;
    temp->priority = newProcess.priority;
    temp->lastBlockingTime = newProcess.arrivalTime;
    temp->memorySize = newProcess.memorySize;
    temp->waitingTime = newProcess.waitingTime;
    temp->remainingTime = newProcess.runningTime;
    temp->idleTime = newProcess.idleTime;
    temp->startTime = newProcess.startTime;
    printf("Received %d %d %d %d \n", temp->id, temp->arrivalTime, temp->runningTime, temp->priority);
    printf("--------------- \n");
    switch (type)
    {
    case HPF:
        enqueuePriorityQueue(queueHPF,temp);
        break;
    case STRN:
        enqueuePriorityQueue(queueSTRN,temp);
        printf("Rem time of front %d\n", frontPriorityQueue(queueSTRN)->remainingTime);
        printf("Rem of new process %d \n",temp->remainingTime);
        printf("added new process to queue \n");
        printf("front now is a process with id %d \n", frontPriorityQueue(queueSTRN)->id);
        break;
    case RR:
        enqueueQueue(queueRR, temp);
        break;
    }
    signal(SIGUSR1, newProcessArrived);   
}
void noMoreProcesses(int signum)
{
    finish = true;
    printf("No more processes \n");
    signal(SIGUSR2, noMoreProcesses);
}

void sendProcessParameters(int Q_ID_SMP, int burstTime, int startTime,int waitingTime,int processID)
{
    //Construct the message 
    ProcessParametersBuff message;
    message.mtype = processID%10000; 

    message.parameters.burstTime= burstTime;
    message.parameters.startTime= startTime;
    message.parameters.waitingTime= waitingTime;

    //Send the message
    int send_val = msgsnd(Q_ID_SMP, &message, sizeof(message.parameters), !IPC_NOWAIT);

    //Handle unexpected errors by notifying the user
    if (send_val == -1)
        perror("Errror in sending the initial data to the scheduler.");
}

int forkANewProcess(processData* p)
{
    int newProcessPID = fork();
    
    if (newProcessPID == -1)
  	    perror("error in forking a new process");	
    else if (newProcessPID == 0)
    {
        printf("newProcess \n");
	    execl("/home/mariam/OS_Project/Cool_OS_Project/project/Phase 1 (Scheduler)/code/process.o", "process.o", NULL);
    }
    else 
    {
        p->forkingID = newProcessPID;
        return newProcessPID;
    }
}
void highestPriorityFirst(int Q_ID_SMP,Logger *logger,Memory *memory)
{
    int stat_loc;
    processData* p = dequeuePriorityQueue(queueHPF); //Dequeue the process whose turn is now
    MemoryLocation* address= allocate(memory,p->id,p->memorySize); //Allocate a place in the memory for the process 
    printAddress(address);
    memoryLog(memory,p->id,getClk(),true);
    int newProcessPID=forkANewProcess(p);//Fork the process
    hashInsert(PCB, p->id, p); //Insert the forked process into the PCB
    p->waitingTime=getClk()-p->arrivalTime;
    //Log the process to the file (state=STARTED)
    schedulerLog(logger,getClk(),p->id,STARTED,p->arrivalTime,p->runningTime,p->runningTime,p->waitingTime);
    sendProcessParameters(Q_ID_SMP,p->runningTime, getClk(),0,newProcessPID); //Send the parameters
    wait(&stat_loc); //Wait for the process to finish
    schedulerLog(logger,getClk(),p->id,FINISHED,p->arrivalTime,p->runningTime,0,p->waitingTime);
    memoryLog(memory,p->id,getClk(),false);//deallocate from the memory
    deallocate(memory,p->id);
    hashRemove(PCB, p->id); //Remove the finished process from the PCB
}
void shortestRemainingTimeNext(int Q_ID_SMP,Memory* memory, Logger* logger)
{
    int stat_loc, newProcessPID;
    processData* p = frontPriorityQueue(queueSTRN); //Dequeue the process whose turn is now  
    if (!hashFind(PCB, p->id)) //first time the process executes
    {
        printf("process id: %d first time running \n", p->id);
        MemoryLocation* address= allocate(memory,p->id,p->memorySize);
        printAddress(address);
        memoryLog(memory,p->id,getClk(),true);
        newProcessPID = forkANewProcess(p); //fork the process
        p->waitingTime = getClk() - p->arrivalTime;
        schedulerLog(logger,getClk(),p->id,STARTED,p->arrivalTime,p->runningTime,p->runningTime,p->waitingTime);
        p->startTime = getClk();
        p->forkingID = newProcessPID;
        hashInsert(PCB, p->id, p); 
        sendProcessParameters(Q_ID_SMP,p->runningTime, getClk(),0,newProcessPID);
    }
    else
    {
        kill(p->forkingID,SIGCONT);
        p->idleTime = getClk() - p->lastBlockingTime;
        p->waitingTime += p->idleTime;
        printf("process id: %d resumed \n", p->id);
        sendProcessParameters(Q_ID_SMP,p->runningTime, p->startTime,p->idleTime,p->forkingID);
        schedulerLog(logger,getClk(),p->id,RESUMED,p->arrivalTime,p->runningTime,p->remainingTime,p->waitingTime);
    }
    while (p == frontPriorityQueue(queueSTRN) && (p->remainingTime = p->runningTime - getClk() + p->startTime + p->idleTime) > 0);
    if (p->remainingTime == 0 && p == frontPriorityQueue(queueSTRN))
    {
        schedulerLog(logger,getClk(),p->id,FINISHED,p->arrivalTime,p->runningTime,p->remainingTime,p->waitingTime);
        printf("process id %d  is done\n", p->id);
        dequeuePriorityQueue(queueSTRN);
        hashRemove(PCB, p->id);
        memoryLog(memory,p->id,getClk(),false);//deallocate from the memory
        deallocate(memory,p->id);   
    } 
    else
    {
        printf("process id: %d stopped \n", p->id);
        p->lastBlockingTime = getClk();
        schedulerLog(logger,getClk(),p->id,STOPPED,p->arrivalTime,p->runningTime,p->remainingTime,p->waitingTime);
        kill(p->forkingID,SIGSTOP); //process that has been removed from front should sleep
        printf("Context switching occurred to pid = %d. \n",frontPriorityQueue(queueSTRN)->id);
    }
}
int main(int argc, char * argv[])
{
    signal(SIGUSR1, newProcessArrived);
    signal(SIGUSR2, noMoreProcesses);
    initClk();
    Logger *logger=createLogger();
    Memory *memory=createMemory();
    printf("initial %d ", getClk());
    //Create a message Queue for sending the initial data(scheduling type, parameters,number of processes)
    int INITIAL_MSG_Q_ID = msgget(55, 0666 | IPC_CREAT); 

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
        perror("Error in create sched");
        exit(-1);
    }
    else
    {
        printf("Shared memory ID scheduler sched: %d \n", shmid);
    }

    //Message Queue for the processes
    int Q_ID_SMP = msgget(Q_ID_SMP_KEY, 0666 | IPC_CREAT); 
    //Handle unexpected errors by notifying the user and shutting down
    if (Q_ID_SMP == -1)
    {
        perror("Error in create the message queue of schedulerIsMessagingProcess");
        exit(-1);
    }
    //schedulingAlgorithm type = RR;
    schedulingType s = receiveSchedulingType(INITIAL_MSG_Q_ID);
    type = s.algo;
    int quantum = s.quantum;
    PCB = createHash(s.numProcesses * 3);
   
    printf("alg %d, q %d , n %d \n", type, s.quantum, s.numProcesses);

    //Creating the appropriate queue for the choosen algorithm
    switch (type)
    {
    case HPF:

        queueHPF = createPriorityQueue(PRIORITIZE_PRIORITY);
        break;
    case STRN:
        queueSTRN = createPriorityQueue(PRIORITIZE_TIME);
        printf("created queue for srtn\n");
        break;
    case RR:
        queueRR = createQueue();
        break;
    }

    //Should give the hash a size
    ///TODO: ( Replace with getcloack() )
    int clock = getClk(); 
    //printf("hash size %d \n", PCB->count);

    //Sarting the scheduling process
    switch (type)
    {
    case HPF:
        while(1){
            while(!isEmptyPriorityQueue(queueHPF))
            {
                 highestPriorityFirst(Q_ID_SMP,logger,memory);
            }
            if (finish)
                break;
        }
        break;
    case STRN:
        while(1){
            while(!isEmptyPriorityQueue(queueSTRN))
            {
                shortestRemainingTimeNext(Q_ID_SMP,memory,logger);
            }
            if (finish)
                break;
        }
        break;
    case RR:
        while(1){
            while(!isEmptyQueue(queueRR))
            {
                int prev = getClk();
                printf("prev %d \n", prev);
                // doATestHPF(Q_ID_SMP);
                if(doRR(getClk(),queueRR,PCB, quantum)) exit(0);
                int now = getClk();
                while (now - prev < 1)
                {
                    printf("now %d \n", now);
                    now = getClk();
                }
            }
            if (finish)
                break;
            else
                sleep;
        }
        break;
    default:
        break;
    }
    //This should be called after all the processes have finished
    schedulerPerf(logger);
    destroyLogger(logger);
    destroyMemory(memory);
    destroyClk(true);

    return 0;
}
