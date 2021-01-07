#include "headers.h"
#include "priorityQueue.c"


int main(int argc, char * argv[])
{
    struct PriorityQueue* priorityQ=createPriorityQueue(PRIORITIZE_TIME);
    processData process1={10,10,10,10};
    processData process2={5,5,5,5};
    processData process3={3,3,3,3};
    processData process4={11,11,11,11};

    enqueue(priorityQ,&process1);
    enqueue(priorityQ,&process2);
    enqueue(priorityQ,&process3);
    enqueue(priorityQ,&process4);

    printf("%d\n",dequeue(priorityQ)->arrivaltime);
    printf("%d\n",dequeue(priorityQ)->arrivaltime);
    printf("%d\n",dequeue(priorityQ)->arrivaltime);
    printf("%d\n",dequeue(priorityQ)->arrivaltime);
}