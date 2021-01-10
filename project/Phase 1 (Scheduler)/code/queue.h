//#include <stdio.h> 
//#include <stdlib.h> 
#include "headers.h"

struct Queue;


//Queue functions
struct Queue* createQueue();
processData* front(struct Queue* self);
bool enqueueQueue(struct Queue* self, processData* d);
processData* dequeueQueue(struct Queue* self);
bool isFullQueue(struct Queue* self);
bool isEmptyQueue(struct Queue* self);
void resetQueue(struct Queue* self);
void destroyQueue(struct Queue* self);


