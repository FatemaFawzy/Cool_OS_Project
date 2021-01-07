#include "headers.h"
struct PriorityQueue;  // forward declared for encapsulation
struct PriorityQueue* createPriorityQueue();  // equivalent to "new PriorityQueue(x, y)"
void destroyPriorityQueue( struct PriorityQueue* self);  // equivalent to "delete PriorityQueue"
bool enqueue( struct PriorityQueue* self, processData* process );  // equivalent to "PriorityQueue->x()"
processData* dequeue(struct PriorityQueue* self );  // equivalent to "PriorityQueue->y()"