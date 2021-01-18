#include "headers.h"

int main()
{

    signal (SIGINT, handler);                       //define the signal handler for termination

    initialize();                                   //create shared buffer and buffer semaphores (mutex, full, empty)

    down(countMutex);                               //mutex lock
    int count= reader(countShmaddr,0);              //read the current processes count
    writer(countShmaddr,count+1,0);                 //increment the count
    up(countMutex);                                 //mutex unlock

    int indextoRead, consumedValue, prevIndex;

    while(1) {

            down(full);                                     //check if the buffer isn't empty
            down(mutex);                                    //mutex lock to read from shared memory

            indextoRead=reader(indexShmaddr_c,0);           //read the index to start at (where the last consumer stopped)
            prevIndex=indextoRead;                          //store the index to be used in the printf

            consumedValue=reader(shmaddr,indextoRead);      //read from current buffer index

            indextoRead= (indextoRead+1) % N;               //update the index to the next location of reading from the buffer
            writer(indexShmaddr_c,indextoRead,0);           //save the index to the shared memory to be accessed by other consumers   

            up(mutex);                                      //mutex unlock
            up(empty);                                      //notify the producer that an item was consumed from the buffer through sempaphores

            printf("\n Consumed from buffer index: %d value: %d\n", prevIndex, consumedValue);  //print consumed value
            sleep(1);
    }

    return 0;
}