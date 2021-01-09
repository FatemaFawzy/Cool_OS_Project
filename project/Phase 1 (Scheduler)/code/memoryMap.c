#include "headers.h"

typedef struct 
{
    int id;
    int startAddress;
    int endAddress;
}MemoryLocation;

struct MemoryMapNode{
    MemoryLocation* value;
    struct MemoryMapNode* next;
};

typedef struct{
    int size;
    int count;
    struct MemoryMapNode** table;
}MemoryMap;


// Constructor
void initMemoryMap(MemoryMap* self,int size){
    self->size = size;
    self->count = 0;    
    self->table = (struct MemoryMapNode**)malloc(sizeof(struct MemoryMapNode*)*size);
}
MemoryMap* createMemoryMap(int size){
    if(size >MAX_SIZE)
        return NULL;
    MemoryMap* table= (MemoryMap*)malloc(sizeof(MemoryMap));
    initMemoryMap(table,size);
    return table;
}
// Destructor
void resetMemoryMap(MemoryMap* self){
    free(self->table);
}
void destroyMemoryMap(MemoryMap* self)
{
    if(self)
    {
        resetMemoryMap(self);
        free(self);
    }
}
// Size functions
int memoryMapSize(MemoryMap* self)
{
    return self->size;
}
bool memoryMapResize(MemoryMap* self,int size)
{
    if(size >MAX_SIZE)
        return false;

    int* newKeys = (int*)malloc(sizeof(int)*size);
    struct MemoryMapNode**  newTable= (struct MemoryMapNode**)malloc(sizeof(struct MemoryMapNode*)*size);

    memcpy(newTable,self->table,self->size);

    free(self->table);
    self->table = newTable;
    self->size= size;
    return true;
}
bool isMemoryMapFull(MemoryMap* self)
{
    return self->size == self->count;
}
bool isMemoryMapEmpty(MemoryMap* self)
{
    return self->count == 0;
}
// memoryMapHash functions 
unsigned int memoryMapHashFunction(unsigned int key)
{
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key;
}

unsigned int memoryMapHash(MemoryMap* self,unsigned int key)
{
    return memoryMapHashFunction(key) % self->size;
}

// Operation functions
MemoryLocation* memoryMapFind(MemoryMap*self, unsigned int key)
{
    int i = memoryMapHash(self,key);
    struct MemoryMapNode* root= self->table[i] ;
    while(root != NULL)
    {
        if(root->value->id == key)
            return root->value;
        root = root->next;
    }
    return NULL;
}
bool memoryMapInsert(MemoryMap*self, unsigned int key,MemoryLocation* process)
{
    if(self->count == self->size)
        return false;
    int i = memoryMapHash(self,key);
    struct MemoryMapNode* root= self->table[i];

    if(root == NULL)
    {
        root = (struct MemoryMapNode*) malloc(sizeof(struct MemoryMapNode));
        root->value = process;
        self->table[i] = root;
        self->count++;
        return true;
    }

    while(root->next != NULL) root = root->next;
    root->next = (struct MemoryMapNode*) malloc(sizeof(struct MemoryMapNode));
    (root->next)->value = process;
    self->count++;
    return true;
}
MemoryLocation* memoryMapRemove(MemoryMap* self,unsigned int key)
{
    if(self->count == 0) return false;

    int i = memoryMapHash(self,key);
    struct MemoryMapNode* root= self->table[i] ;

    struct MemoryMapNode* prev= NULL;

    while(root != NULL)
    {
        if(root->value->id == key)
        {
            if(prev != NULL)
                prev->next = root->next;

            if(self->table[i] == root)
                self->table[i] = root->next;
            MemoryLocation* process = root->value;
            self->count--;
            free(root);
            root = NULL;
            return process;
        }
        prev = root;
        root = root->next;
    }
    return NULL;
}

