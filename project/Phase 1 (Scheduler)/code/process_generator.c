#include "headers.h"

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE* inputFile = fopen("processes.txt","r");
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

    // 4. Use this function after creating the clock process to initialize clock
    //initClk();
    // To get time use this
    //int x = getClk();
    //printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    //Queue processQueue;
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
        p->arrivaltime = atoi(token2);
        char * token3 = strtok(NULL, "\t");
        p->runningtime = atoi(token3);
        char * token4 = strtok(NULL, "\t");
        p->priority = atoi(token4);
        printf("%d %d %d %d \n", p->id, p->arrivaltime, p->runningtime, p->priority);
    }
    //inserted = processQueue.enqueue(&p);
    //if (!inserted)
    //{
    //printf("Reached max number of processes!");
    //break;
    //}
    // 6. Send the information to the scheduler at the appropriate time.
    bool sentAllProcesses = false;
    //int numProcesses = processQueue.size;
    int counter = 0;
    //while (!sentAllProcesses)
    //{
        //int now = getClk();
        //while (now < processQueue.front()->arrivalTime) //send to scheduler
        //{
        //now = getClk();
        //}
        //if we reach this point, then arrival time is now
        //processData * processToBeSent = processQueue.dequeue();
        //if (processToBeSent != NULL)
        //{
        //send to scheduler
        //counter++;
        //}
        //if (counter == numProcesses)
        //sentAllProcesses = true;

    //}
    // 7. Clear clock resources
    //destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

