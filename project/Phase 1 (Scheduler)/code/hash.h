#include <string.h>
#include "headers.h"
#define DEFAULT_HASH_SIZE 10



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
int sizeHash(struct Hash* self);
bool resizeHash(struct Hash* self,int size);
bool isFullHash(struct Hash* self);
bool isEmptyHash(struct Hash* self);
// Hash functions 
unsigned int hash(struct Hash* self,unsigned int key);
unsigned int hashFunction(unsigned int key);
// Operation functions
processData* hashFind(struct Hash*self, unsigned int key);
bool hashInsert(struct Hash* self, unsigned int key,processData* process);
processData* hashRemove(struct Hash* self,unsigned int key);
