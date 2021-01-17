#ifndef HEADERS
#define HEADERS

#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


typedef short bool;

#define true 1
#define false 0

#define SHKEY 300
#define Q_ID_SMP_KEY 99
#define SEM_ID_PG_TO_SCH 44

typedef struct
{
    int id;
    int arrivalTime;
    int runningTime; 
    int priority;
    int memorySize; 

    int remainingTime;
    int waitingTime; 
    int lastBlockingTime;
    int forkingID;
    int startTime;
    int idleTime;
    int enteredWaitingListTime;
} processData;


struct Node {
    processData* data;
    struct Node* next;
};

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

#define MAX_SIZE 1000

typedef enum {PRIORITIZE_TIME,PRIORITIZE_PRIORITY} PriorityPurpose; 
typedef enum {STARTED,RESUMED,STOPPED,FINISHED} ProcessState; 
typedef enum {HPF=1,STRN=2,RR=3} schedulingAlgorithm;
///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================


typedef struct 
{
    schedulingAlgorithm algo;
    int quantum; 
    int numProcesses;
} schedulingType;

typedef struct 
{
    long mtype;
    schedulingType initialSchedulingData;
    
} msgbuff;

typedef struct 
{
    int burstTime;
    int startTime;
    int waitingTime;
}ProcessParameters;

typedef struct 
{
    long mtype;
    ProcessParameters parameters;
    
} ProcessParametersBuff;

int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}
int power(int base, int exp)
{
    int result = 1;
    while(exp) { result *= base; exp--; }
    return result;
}

#endif
