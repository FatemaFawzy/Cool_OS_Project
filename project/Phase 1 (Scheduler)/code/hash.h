#include <string.h>
#include "headers.h"
#define DEFAULT_HASH_SIZE 10



struct HashNode
{
    PCBEntry* value;
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
PCBEntry* hashFind(struct Hash*self, unsigned int key);
bool hashInsert(struct Hash* self, unsigned int key,PCBEntry* process);
PCBEntry* hashRemove(struct Hash* self,unsigned int key);
