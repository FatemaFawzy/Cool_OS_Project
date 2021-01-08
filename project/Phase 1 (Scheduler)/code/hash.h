#include <string.h>
#include "../Phase 1 (Scheduler)/code/headers.h"
#define DEFAULT_HASH_SIZE 10
typedef struct 
{
    int arrivalTime;
    int priority;
    int runningTime;
    // int remainingTime;
    // int waitingTime; 
    // int cummulativeRunningTime;    
    // int programCounter;                                                                           
    int id;
} processData; 


struct HashNode
{
    processData* value;
    struct HashNode* next;
};

struct Hash;
// Constructor
void initHash(struct Hash* self,int size);
struct Hash* createHash(int size);
// Destructor
void resetHash(struct Hash* self);
void destroyHash(struct Hash* self);
// Size functions
int size(struct Hash* self);
bool resize(struct Hash* self,int size);
bool isFull(struct Hash* self);
bool isEmpty(struct Hash* self);
// Hash functions 
unsigned int hash(struct Hash* self,unsigned int key);
unsigned int hashFunction(unsigned int key);
// Operation functions
processData* hashFind(struct Hash*self, unsigned int key);
bool hashInsert(struct Hash* self, unsigned int key,processData* process);
processData* hashRemove(struct Hash* self,unsigned int key);
