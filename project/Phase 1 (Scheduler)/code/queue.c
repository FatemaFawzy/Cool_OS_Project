#include "queue.h"

struct Queue{
    struct Node* front;
    struct Node* rear;
    int size;
} ;

//create a new queue
struct Queue* createQueue(){

    struct Queue* queue= (struct Queue*) malloc(sizeof(struct Queue));
    queue->front= NULL;
    queue->rear=NULL;
    return queue;

}

//read data from the front of the queue
processData* front(struct Queue* self){

    if(self->front==NULL) return NULL;
    else {
        return self->front->data;
    }

}

//add data to queue
bool enqueue(struct Queue* self, processData* d){

    //if maximum queue capacity is reached, don't enqueue
    if(isFull(self)) return false;

    else {
        //create a new node and insert the data in it
        struct Node* node= (struct Node*) malloc(sizeof(struct Node));
        node->data=d;
        node->next=NULL;

        //if queue is empty, insert at front
        if(self->front==NULL){
            self->front=node;
            self->rear=node;
        }
        //else, insert after the rear and set the inserted node as the rear
        else {
            self->rear->next=node;
            self->rear=node;
        }

        self->size++;
        return true;
    }

}

//remove data from queue front
processData* dequeue(struct Queue* self){

    if(self->front == NULL) return NULL;
    else {
        struct Node* temp= self->front;

        //check if the queue has only 1 element
        if(self->front==self->rear) {
            self->rear=NULL;
            self->front=NULL;
        }
        else {
            self->front=self->front->next;
        }

        //deallocate deleted node and return its data
        processData* data= temp->data;
        free(temp);
        self->size--;
        return data;
    }

}

//check if queue is full
bool isFull(struct Queue* self){

    if(self->size ==MAX_SIZE) return true;
    else return false;

}

//check if queue is empty
bool isEmpty(struct Queue* self){

    if(self->size ==0) return true;
    else return false;

}

//deallocate queue nodes
void resetQueue(struct Queue* self){

    struct Node* curr= self->front;
    struct Node* temp;
    while(curr!=NULL){
        temp=curr;
        curr=curr->next;
        free(temp);
    }

}

//destroy current queue
void destroyQueue(struct Queue* self){

    if(self) {
        resetQueue(self);
        free(self);
    }

}