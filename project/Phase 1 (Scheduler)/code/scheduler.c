#include "priorityQueue.c"
#include "hash.c"

#define PROCESSES_NUMBER 5



void insertProcess(struct PriorityQueue* queue,PCBEntry* process,int clock)
{
    if(process != NULL && process->arrivalTime == clock)
    {
        printf("\nat t=%d: Process with ID %d has arrived!\n",clock,process->id);
        enqueue(queue,process);    
    }
}

// Highest priority first
bool doHPF(int clock,struct PriorityQueue* queue,struct Hash* PCB)
{
    // Initializing
    static int processessCounter = 0;
    static PCBEntry* process = NULL;
    //Check if new process comes 
    ///TODO:(SHOULD BE REPLACED)
    insertProcess(queue,hashFind(PCB,1),clock);
    insertProcess(queue,hashFind(PCB,2),clock);
    insertProcess(queue,hashFind(PCB,3),clock);
    insertProcess(queue,hashFind(PCB,4),clock);
    insertProcess(queue,hashFind(PCB,5),clock); 

    if(!isEmpty(PCB)){
        // Get current process & process info
        if(process == NULL) process = dequeue(queue);
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
                printf("\n--> Process %d with priority %d has finished in t=%d after waiting %ds <--\n",process->id,process->priority,clock,process->waitingTime);
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

// Shortest TIme ramining next
bool doSTRN(int clock,struct PriorityQueue* queue,struct Hash* PCB){
    // Initializing
    static PCBEntry* prev = NULL; 
    static int processessCounter = 0;

    //Check if new process comes 
    ///TODO:(SHOULD BE REPLACED)
    insertProcess(queue,hashFind(PCB,1),clock);
    insertProcess(queue,hashFind(PCB,2),clock);
    insertProcess(queue,hashFind(PCB,3),clock);
    insertProcess(queue,hashFind(PCB,4),clock);
    insertProcess(queue,hashFind(PCB,5),clock); 

    if(!isEmptyPriorityQueue(queue)){
        // Get current process & process info
        PCBEntry* process = front(queue);

        // Checking on context switching (DEBUGGING PURPOSES)
        if( process != prev)
        {
            if(prev != NULL) printf("\nAt t=%d: Process %d with RT=%d has swtiched the context to process %d with RT=%d\n",clock,prev->id,prev->remainingTime,process->id,process->remainingTime);
            else printf("\nAt t=%d: Switching the context to process %d with RT=%d\n",clock,process->id,process->remainingTime);
            prev= process;
        }
        // Calculate waiting time for this process
        process->waitingTime += clock - process->lastBlockingTime;

        // Move remaining time
        process->remainingTime --;
        if(process->remainingTime == 0)
        {
            printf("\n--> Process %d with running time %d has finished in t=%d after waiting %ds <--\n",process->id,process->runningTime,clock,process->waitingTime);
            dequeue(queue);
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
  

int main(int argc, char * argv[])
{
    //initClk();
    schedulingAlgorithm type = HPF;

    //Should give the hash a size
    struct Hash* PCB = createHash(10);

    ///TODO: (Replace this with taking input from Process generator)
    // id arrival runtime priority memroy remaining waiting lastBlocking
    static PCBEntry pcb1 = {1,16,23,2,100,  23,0,16};
    static PCBEntry pcb2 = {2,3,18,1,100,  18,0,3};
    static PCBEntry pcb3 = {3,11,24,10,100,  24,0,11};
    static PCBEntry pcb4 = {4,23,16,7,100,  16,0,23};
    static PCBEntry pcb5 = {5,26,9,4,100,  9,0,26};
    //should loop on these ..
    hashInsert(PCB,pcb1.id,&pcb1);
    hashInsert(PCB,pcb2.id,&pcb2);
    hashInsert(PCB,pcb3.id,&pcb3);
    hashInsert(PCB,pcb4.id,&pcb4);
    hashInsert(PCB,pcb5.id,&pcb5);

    ///TODO: ( Replace with getcloack() )
    int clock = 0; 
    

    //Creating the appropriate queue for the choosen algorithm
    struct PriorityQueue* queueHPF;
    struct PriorityQueue* queueSTRN;
        //struct PriorityQueue* queueRR = createQueue();
    switch (type)
    {
    case HPF:
        queueHPF = createPriorityQueue(PRIORITIZE_PRIORITY);
        break;
    case STRN:
        queueSTRN = createPriorityQueue(PRIORITIZE_TIME);
        break;
    case RR:
        /* code */
        break;
    }

    //Sarting the scheduling process
    while(1){
        switch (type)
        {
        case HPF:
            if(doHPF(clock,queueHPF,PCB)) exit(0);
            break;
        case STRN:
            if(doSTRN(clock,queueSTRN,PCB)) exit(0);
            break;
        case RR:
            /* code */
            break;
        }
        
        // Clock incearsing
        clock++;
    }

    return 0;
   // destroyClk(true);
}
