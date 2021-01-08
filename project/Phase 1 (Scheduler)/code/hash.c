#include "hash.h"

struct Hash{
    int size;
    int count;
    struct HashNode** table;
};

// Constructor
void initHash(struct Hash* self,int size){
    self->size = size;
    self->count = 0;    
    self->table = (struct HashNode**)malloc(sizeof(struct HashNode*)*size);
}
struct Hash* createHash(int size){
    if(size >MAX_SIZE)
        return NULL;
    struct Hash* table= (struct Hash*)malloc(sizeof(struct Hash));
    initHash(table,size);
    return table;
}
// Destructor
void resetHash(struct Hash* self){
    free(self->table);
}
void destroyHash(struct Hash* self)
{
    if(self)
    {
        resetHash(self);
        free(self);
    }
}
// Size functions
int size(struct Hash* self)
{
    return self->size;
}
bool resize(struct Hash* self,int size)
{
    if(size >MAX_SIZE)
        return false;

    int* newKeys = (int*)malloc(sizeof(int)*size);
    struct HashNode**  newTable= (struct HashNode**)malloc(sizeof(struct HashNode*)*size);

    memcpy(newTable,self->table,self->size);

    free(self->table);
    self->table = newTable;
    self->size= size;
    return true;

}
bool isFull(struct Hash* self)
{
    return self->size == self->count;
}
bool isEmpty(struct Hash* self)
{
    return self->count == 0;
}
// Hash functions 
unsigned int hash(struct Hash* self,unsigned int key)
{
    return hashFunction(key) % self->size;
}
unsigned int hashFunction(unsigned int key)
{
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key;
}
// Operation functions
PCBEntry* hashFind(struct Hash*self, unsigned int key)
{
    int i = hash(self,key);
    struct HashNode* root= self->table[i] ;
    while(root != NULL)
    {
        if(root->value->id == key)
            return root->value;
        root = root->next;
    }
    return NULL;
}
bool hashInsert(struct Hash*self, unsigned int key,PCBEntry* process)
{
    if(self->count == self->size)
        return false;
    int i = hash(self,key);
    struct HashNode* root= self->table[i];

    if(root == NULL)
    {
        root = (struct HashNode*) malloc(sizeof(struct HashNode));
        root->value = process;
        self->table[i] = root;
        self->count++;
        return true;
    }

    while(root->next != NULL) root = root->next;
    root->next = (struct HashNode*) malloc(sizeof(struct HashNode));
    (root->next)->value = process;
    self->count++;
    return true;
}
PCBEntry* hashRemove(struct Hash* self,unsigned int key)
{
    if(self->count == 0) return false;

    int i = hash(self,key);
    struct HashNode* root= self->table[i] ;

    struct HashNode* prev= NULL;

    while(root != NULL)
    {
        if(root->value->id == key)
        {
            if(prev != NULL)
                prev->next = root->next;

            if(self->table[i] == root)
                self->table[i] = root->next;
            PCBEntry* process = root->value;
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

