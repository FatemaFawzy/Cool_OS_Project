#include "priorityQueue.h"

// Source file
struct PriorityQueue  {
   PCBEntry* data[MAX_SIZE];
   int size;
   PriorityPurpose purpose; 
};

// Constructor (without allocation)
void PriorityQueue__init(struct PriorityQueue* self,PriorityPurpose purpose) {
  self->size = 0;
  self->purpose=purpose;
  for(int i=0;i<MAX_SIZE;i++)
    self->data[i]=NULL;
 }

// Allocation + initialization (equivalent to "new PriorityQueue(x, y)")
struct PriorityQueue* createPriorityQueue(PriorityPurpose purpose) {
   struct PriorityQueue* result = (struct PriorityQueue*) malloc(sizeof(struct PriorityQueue));
   PriorityQueue__init(result,purpose);
   return result;
}

// Destructor (without deallocation)
void resetPriorityQueue(struct PriorityQueue* self) {
}

// Destructor + deallocation (equivalent to "delete point")
void destroyPriorityQueue(struct PriorityQueue* queue) {
  if (queue) {
     resetPriorityQueue(queue);
     free(queue);
  }
}

//Helper Private Functions for the priority queue

void minHeapify (PCBEntry* data[] , int i, int N,PriorityPurpose purpose)
{
int left  = 2*i;
int right = 2*i+1;
int smallest;
if(purpose==PRIORITIZE_TIME)
{
    if(left <= N && data[left]->remainingTime < data[ i ]->remainingTime )
     smallest = left;
else
    smallest = i;
if(right <= N && data[right]->remainingTime < data[smallest]->remainingTime )
    smallest = right;
if(smallest != i)
{
    PCBEntry* tempProcess=data[ i ];
    data[ i ]=data[ smallest ];
    data[ smallest ]=tempProcess;
    minHeapify (data, smallest,N,purpose);
} 
}
else
{
if(left <= N && data[left]->priority < data[ i ]->priority )
    smallest = left;
else
    smallest = i;
if(right <= N && data[right]->priority < data[smallest]->priority )
    smallest = right;
if(smallest != i)
{
    //Swap
    PCBEntry* tempProcess=data[ i ];
    data[ i ]=data[ smallest ];
    data[ smallest ]=tempProcess;
    minHeapify (data, smallest,N,purpose);
}   
}
}

bool decreaseValue(PCBEntry* data[] , int i,PCBEntry* process, PriorityPurpose purpose)
{

    if(purpose==PRIORITIZE_TIME)
    {
            // if(process->remainingTime > data[ i ]->remainingTime)
            //     return false;
        data[ i ] = process;
        while( i > 1 && data[ i/2 ]->remainingTime > data[ i ]->remainingTime)
        {
            //Swap
            PCBEntry* tempProcess=data[ i ];
            data[ i ]=data[ i/2 ];
            data[ i/2 ]=tempProcess;
            i = i/2;
        }

    }else{
            // if(process->priority > data[ i ]->priority)
            //     return false;
        data[ i ] = process;
        while( i > 1 && data[ i/2 ]->priority > data[ i ]->priority)
        {
            //Swap
            PCBEntry* tempProcess=data[ i ];
            data[ i ]=data[ i/2 ];
            data[ i/2 ]=tempProcess;
            i = i/2;
        }
    }
}

// Equivalent to "Point::x()" in C++ version
bool enqueue(struct PriorityQueue* self,PCBEntry* process) {
    if(self->size==MAX_SIZE) return false;
    if(self->size==0)
    {
        self->data[ ++self->size ]=process;
        return true;
    } 
    self->size += 1;
    // if(self->purpose==PRIORITIZE_TIME)
    //     self->data[ self->size ]->remainingTime = __INT_MAX__;  
    // else  self->data[ self->size ]->priority = __INT_MAX__;
    return decreaseValue (self->data, self->size, process,self->purpose);
}

// Equivalent to "Point::y()" in C++ version
PCBEntry* dequeue(struct PriorityQueue* self) {
    if(self->size == 0)
        return NULL;
    PCBEntry* dequeuedProcess = self->data[1];
    self->data[1] =self->data[self->size];
    self->data[self->size]=NULL;
    self->size -= 1;
    minHeapify(self->data, 1,self->size, self->purpose);
    return dequeuedProcess;
}

PCBEntry* front(struct PriorityQueue* self) {
    if(self->size==0) return NULL;
    return self->data[1];
}

bool isEmptyPriorityQueue(struct PriorityQueue* self)
{
    return self->size==0;
}

// bool isFull(struct PriorityQueue* self)
// {
//     return self->size==MAX_SIZE;
// }