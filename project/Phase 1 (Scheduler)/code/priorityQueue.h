#include "headers.h"

struct PriorityQueue;

// Constructor (without allocation)
void PriorityQueue__init(struct PriorityQueue* self,PriorityPurpose purpose);

// Allocation + initialization (equivalent to "new PriorityQueue(x, y)")
struct PriorityQueue* createPriorityQueue(PriorityPurpose purpose);

// Destructor (without deallocation)
void resetPriorityQueue(struct PriorityQueue* self);
// Destructor + deallocation (equivalent to "delete point")
void destroyPriorityQueue(struct PriorityQueue* queue);

//Helper Private Functions for the priority queue

// void minHeapify (processData* data[] , int i, int N,PriorityPurpose purpose);

// bool decreaseValue(processData* data[] , int i,processData* process, PriorityPurpose purpose);

// Equivalent to "Point::x()" in C++ version
bool enqueuePriorityQueue(struct PriorityQueue* self,processData* process);

// Equivalent to "Point::y()" in C++ version
processData* dequeuePriorityQueue(struct PriorityQueue* self);
processData* frontPriorityQueue(struct PriorityQueue* self);



bool isEmptyPriorityQueue(struct PriorityQueue* self);
bool isFullPriorityQueue(struct PriorityQueue* self);