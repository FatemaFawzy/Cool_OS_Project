#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//buffer size
#define N 10

typedef int semaphore;

//global variables
int shmid, indexShmid_p, indexShmid_c, countShmid;
void *shmaddr;
void *countShmaddr;
void* indexShmaddr_p;
void* indexShmaddr_c;
union Semun semun;
semaphore mutex, full, empty, countMutex;

//------------------------------Semaphore functions---------------------------//

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

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

//---------------------------Shared Memory functions---------------------------//

void writer(void *shmaddr, int val, int i)
{
    
    if (shmaddr == (void *)-1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    
    else
    {   
        //write to shared memory
        ((int *)shmaddr)[i]= val;

        //print buffer index and its content
        //printf("\n Writing to buffer index: %d value: %d\n", i, ((int *)shmaddr)[i]);
    }

}

int reader(void *shmaddr, int i)
{

    if (shmaddr == (void *)-1)
    {
        perror("Error in attach in client reader");
        exit(-1);
    }
    
    //print buffer index and its content
    //printf("\n Reading from buffer index: %d value: %d\n", i, ((int *)shmaddr)[i]);

    return ((int *)shmaddr)[i];
    
}

//--------------------------------initialization--------------------------------//

void initialize() {

    //-----------------------------------Shared memory creation------------------------------//

    //---------Shared buffer-----------//
    //generate a shared memory buffer (using ftok to make sure that the consumer and producer are sharing the same shmadd)
    key_t id= ftok("keyfile",80);
    shmid= shmget(id, N*sizeof(int), IPC_CREAT | 0644);
    shmaddr = shmat(shmid, (void *)0, 0);                                                   //attach to shared memory
    printf("\nBuffer shared memory id %d attached at address %x\n", shmid, shmaddr);       //print shared memory address

    //---------producer index-----------//
    //generate a shared memory location for the index to produce in to be shared with all producers (and consumers as well so that the shared memory can be destroyed if the last process was a consumer)
    key_t indexId_p= ftok("keyfile",90);
    indexShmid_p= shmget(indexId_p, sizeof(int), IPC_CREAT | 0644);
    indexShmaddr_p = shmat(indexShmid_p, (void *)0, 0);                                                             //attach to shared memory
    printf("\nProducer index shared memory id %d attached at address %x\n", indexShmid_p, indexShmaddr_p);          //print shared memory address

    //---------consumer index-----------//
    //generate a shared memory location for the index to consume from to be shared with all consumers (check producer index comment)
    key_t indexId_c= ftok("keyfile",100);
    indexShmid_c= shmget(indexId_c, sizeof(int), IPC_CREAT | 0644);
    indexShmaddr_c = shmat(indexShmid_c, (void *)0, 0);                                                             //attach to shared memory
    printf("\nConsumer index shared memory id %d attached at address %x\n", indexShmid_c, indexShmaddr_c);          //print shared memory address

    //---------Processes count-----------//
    //this memory will be used to keep track of the number of consumers and producers in the system to free all IPC resources on termination of the last process
    //generate a shared memory address (using ftok to make sure that the consumer and producer are sharing the same shmadd)
    key_t countId= ftok("keyfile",30);
    countShmid= shmget(countId, sizeof(int), IPC_CREAT | 0644);                     
    countShmaddr = shmat(countShmid, (void *)0, 0);                                                             //attach to shared memory
    printf("\nProcesses count shared memory id %d attached at address %x\n", countShmid, countShmaddr);         //print shared memory address

    //-----------------------------------Semaphores generation------------------------------//

    key_t mutex_id= ftok("keyfile",70);         //mutual exclusion semaphore to protect the shared buffer
    key_t empty_id= ftok("keyfile",60);         //empty sempahore indicating the number of empty slots in the buffer
    key_t full_id= ftok("keyfile",50);          //full sempahore indicating the number of full slots in the buffer
    key_t countMutex_id= ftok("keyfile",40);    //countMutex semaphore for updating the count of producers and consumers in the shared memory

    //check if the semaphores already exist
    mutex = semget(mutex_id, 1, 0666 | IPC_CREAT | IPC_EXCL);
    empty = semget(empty_id, 1, 0666 | IPC_CREAT | IPC_EXCL);
    full = semget(full_id, 1, 0666 | IPC_CREAT | IPC_EXCL);
    countMutex= semget(countMutex_id, 1, 0666 | IPC_CREAT | IPC_EXCL);

    //if semaphores already exist, retrieve their ids
    if (mutex == -1 || empty == -1 || full == -1 || countMutex == -1)
    {
        mutex = semget(mutex_id, 1, 0666 | IPC_CREAT);
        empty = semget(empty_id, 1, 0666 | IPC_CREAT);
        full = semget(full_id, 1, 0666 | IPC_CREAT);
        countMutex= semget(countMutex_id, 1, 0666 | IPC_CREAT);

        if (mutex == -1 || empty == -1 || full == -1 || countMutex == -1)
        {
            perror("Error in retrieving semaphore ids");
            printf("countMutex = %d", countMutex);
            exit(-1);
        }

    }

    //if they don't exist, initialize the newly created semaphores
    else {

        semun.val = 1; 
        if (semctl(mutex, 0, SETVAL, semun) == -1)
        {
            perror("Error in semctl");
            exit(-1);
        }

        semun.val = N; 
        if (semctl(empty, 0, SETVAL, semun) == -1)
        {
            perror("Error in semctl");
            exit(-1);
        }

        semun.val = 0; 
        if (semctl(full, 0, SETVAL, semun) == -1)
        {
            perror("Error in semctl");
            exit(-1);
        }

        semun.val = 1; 
        if (semctl(countMutex, 0, SETVAL, semun) == -1)
        {
            perror("Error in semctl");
            exit(-1);
        }

    }

}

//-------------------------------Signal handling--------------------------------//

void handler(int signum)
{ 
    
    down(countMutex);                                               //mutex lock to access shared memory (processes count)

    int count= reader(countShmaddr,0);                              //read the current processes count
    
    //if this is the last living process, free all IPC resources
    if(count == 1) {  
                                          
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);              //destroy shared memory (buffer)
        shmctl(indexShmid_p, IPC_RMID, (struct shmid_ds *)0);       //destroy shared memory (indextoWrite)
        shmctl(indexShmid_c, IPC_RMID, (struct shmid_ds *)0);       //destroy shared memory (indextoRead)
        shmctl(countShmid, IPC_RMID, (struct shmid_ds *)0);         //destroy shared memory (processes count)
        semctl(mutex, 0, IPC_RMID, semun);                          //destroy all semaphores
        semctl(full, 0, IPC_RMID, semun);
        semctl(empty, 0, IPC_RMID, semun);
        semctl(countMutex, 0, IPC_RMID, semun);
    }

    //if not, just decrement the count
    else {
        writer(countShmaddr,count-1,0);                             //decrement the count
        up(countMutex);                                             //mutex unlock
    }

    shmdt(shmaddr);                                                 //detach from shared memory (buffer)
    shmdt(indexShmaddr_p);                                          //detach from shared memory (indextoWrite)
    shmdt(indexShmaddr_c);                                          //detach from shared memory (indextoRead)
    shmdt(countShmaddr);                                            //detach from shared memory (processes count)

	raise(SIGKILL);                                                 //destroy self
	
}