#include "priorityQueue.c"
#include "hash.c"
#include "queue.c"
#define PROCESSES_NUMBER 5

int shmid;
struct Hash* PCB;
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
    //Check if new process comes 
    ///TODO:(SHOULD BE REPLACED)
    insertProcessPriorityQueue(queue,hashFind(PCB,1),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,2),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,3),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,4),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,5),clock); 

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

    //Check if new process comes 
    ///TODO:(SHOULD BE REPLACED)
    insertProcessPriorityQueue(queue,hashFind(PCB,1),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,2),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,3),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,4),clock);
    insertProcessPriorityQueue(queue,hashFind(PCB,5),clock); 

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
    msgbuff messageRecieved;

    //recieve only the messages of 50 message type
    int rec_val = msgrcv(INITIAL_MSG_Q_ID, &messageRecieved, sizeof(messageRecieved.initialSchedulingData), 50, !IPC_NOWAIT);

    //Handle unexpected errors by notifying the user
    if (rec_val == -1)
        perror("Error in receiving the initial data from the process generator.");

    return messageRecieved.initialSchedulingData;
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
    printf("Received %d %d %d %d \n", temp->id, temp->arrivalTime, temp->runningTime, temp->priority);
    printf("--------------- \n");
    hashInsert(PCB,newProcess.id,&newProcess);
    signal(SIGUSR1, newProcessArrived);   
}
void noMoreProcesses(int signum)
{
    finish = true;
    printf("No more processes \n");
    signal(SIGUSR2, noMoreProcesses);
}
int main(int argc, char * argv[])
{
    signal(SIGUSR1, newProcessArrived);
    signal(SIGUSR2, noMoreProcesses);
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

    //initClk();
    //schedulingAlgorithm type = RR;
    schedulingType s = receiveSchedulingType(INITIAL_MSG_Q_ID);
    schedulingAlgorithm type = s.algo;
    int quantum = s.quantum;
    PCB = createHash(s.numProcesses * 3);
    printf("alg %d, q %d , n %d \n", type, s.quantum, s.numProcesses);

    //Creating the appropriate queue for the choosen algorithm
    struct PriorityQueue* queueHPF;
    struct PriorityQueue* queueSTRN;
    struct Queue* queueRR;
    switch (type)
    {
    case HPF:
        queueHPF = createPriorityQueue(PRIORITIZE_PRIORITY);
        break;
    case STRN:
        queueSTRN = createPriorityQueue(PRIORITIZE_TIME);
        break;
    case RR:
        queueRR = createQueue();
        break;
    }

    //Should give the hash a size
    ///TODO: ( Replace with getcloack() )
    int clock = 0; 
    //printf("hash size %d \n", PCB->count);

    //Sarting the scheduling process
    while(1){
        while(!isEmptyHash(PCB))
        {
            switch (type)
            {
            case HPF:
                if(doHPF(clock,queueHPF,PCB)) exit(0);
                break;
            case STRN:
                if(doSTRN(clock,queueSTRN,PCB)) exit(0);
                break;
            case RR:
                if(doRR(clock,queueRR,PCB,quantum)) exit(0);
                break;
            }
            ///TODO: Replace it with the original clock
            // Clock incearsing
            clock++;
        }
        if (finish)
            break;
        else
            sleep;
    }

    return 0;
   // destroyClk(true);
}
