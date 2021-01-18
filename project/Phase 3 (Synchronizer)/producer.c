#include "headers.h"

int main()
{
    
    signal (SIGINT, handler);                   //define the signal handler for termination

    initialize();                               //create shared buffer and buffer semaphores (mutex, full, empty)

    down(countMutex);                           //mutex lock
    int count= reader(countShmaddr,0);          //read the current processes count
    writer(countShmaddr,count+1,0);             //increment the count
    up(countMutex);                             //mutex unlock

    int indextoWrite, prevIndex;         

    while(1) {

        //produce a number to be inserted in the queue
        for(int i=100; i<200; i++) {

            down(empty);                                //check if the buffer isn't full
            down(mutex);                                //mutex lock to write in the shared memory
            
            indextoWrite=reader(indexShmaddr_p,0);      //read the index to start at (where the last producer stopped)
            prevIndex=indextoWrite;                     //store the index to be used in the printf

            writer(shmaddr,i,indextoWrite);             //write at the current buffer index

            indextoWrite= (indextoWrite+1) % N;         //update the index to the next location of writing in the buffer  
            writer(indexShmaddr_p,indextoWrite,0);      //save the index to the shared memory to be accessed by other producers

            up(mutex);                                  //mutex unlock                      
            up(full);                                   //notify the consumer that an item was added to the buffer through sempaphores

            printf("\n Produced in buffer index: %d value: %d\n", prevIndex, i);    //print produced value
            sleep(1); 
        }
    }

    return 0;
}