//#include <stdio.h> 
//#include <stdlib.h> 
#include "headers.h"

struct Queue;


//Queue functions
struct Queue* createQueue();
processData* front(struct Queue* self);
bool enqueue(struct Queue* self, processData* d);
processData* dequeue(struct Queue* self);
bool isFull(struct Queue* self);
bool isEmpty(struct Queue* self);
void resetQueue(struct Queue* self);
void destroyQueue(struct Queue* self);


