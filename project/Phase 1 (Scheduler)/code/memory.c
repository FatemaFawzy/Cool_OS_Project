#include "headers.h"
#include "memoryMap.c"

#define NON_EXISTENT -1
#define FREE 0

// Source file
typedef struct{
    int* freeMemory[11];
    MemoryMap *map; 
    FILE *memoryFilePtr;
}Memory;

//helper private functions 

// function to evaluate Binary Logarithm
unsigned int logarithm2(int n) {

    unsigned int myLogValue = -1;
    while (n) {
        myLogValue++;
        n >>= 1;
    }
    return myLogValue;
}

//closest power of 2
unsigned int closestPowerOfTwo(unsigned int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

int subMemorySize(int index)
{
    return power(2,10-index);
}

int subMemoryFree(Memory* self, int subMemoryIndex) 
{
//Return the index of the first free location
//If NO free location, returns NON_EXISTENT
    for(int i=0;i<subMemorySize(subMemoryIndex);i++)
        if(self->freeMemory[subMemoryIndex][i]==FREE)
            return i;
    return NON_EXISTENT;
}

// Conor (without allocation)
void Memory__init( Memory* self) {
    for(int i=10 ;i>=0;i--)
        self->freeMemory[i]=(int*)malloc(sizeof(int)*subMemorySize(i));
    for(int i=0;i<11;i++)
        for(int j=0;j<subMemorySize(i);j++)
            self->freeMemory[i][j]=NON_EXISTENT;
    self->freeMemory[10][0]=FREE;
    
    self->map=createMemoryMap(100);
    self->memoryFilePtr = fopen("memory.log", "w");
 }

// Allocation + initialization (equivalent to "new Memory(x, y)")
 Memory* createMemory(){
    Memory* result = ( Memory*) malloc(sizeof( Memory));
    Memory__init(result);
    return result;
}

// Deor (without deallocation)
void resetMemory( Memory* self) {
    destroyMemoryMap(self->map);
    //Close the file that's used for writing to memory.log
    fclose(self->memoryFilePtr);
}

// Deor + deallocation (equivalent to "delete point")
void destroyMemory( Memory* memory) {
  if (memory) {
     resetMemory(memory);
     free(memory);
  }
}

MemoryLocation* allocate( Memory* self,int proccessID, int size) {
    int allocationStart,allocationEnd;
    int subMemoryIndex = logarithm2(closestPowerOfTwo(size)); 
    int locationFound=subMemoryFree(self,subMemoryIndex);
    if (locationFound !=NON_EXISTENT)  
    { 
        // Allocate
        self->freeMemory[subMemoryIndex][locationFound]=NON_EXISTENT;
        allocationStart=power(2,subMemoryIndex)*locationFound;
        allocationEnd=power(2,subMemoryIndex)*locationFound+power(2,subMemoryIndex)-1;
        MemoryLocation *address= (MemoryLocation *)malloc(sizeof(MemoryLocation));
        address->id=proccessID;
        address->startAddress=allocationStart;
        address->endAddress=allocationEnd;
        memoryMapInsert(self->map,proccessID,address);
        return address;
    } 
    else
    {   //The closest Size is not available
        int largerSize=subMemoryIndex+1;
        int locationOfLarger; 
        while(largerSize<11&& ((locationOfLarger=subMemoryFree(self,largerSize))==NON_EXISTENT))
            largerSize++;

        if (largerSize == 11) //Oops, Memory is FULL
            return NULL;
        else
        {   //Found a larger size, it should be divided now to smaller sizes
            self->freeMemory[largerSize][locationOfLarger]=NON_EXISTENT; //remove the larger one

            //divide into smaller blocks
            int smallerSize=largerSize-1;
            while(smallerSize>=subMemoryIndex)
            {
            //free space in the right block
            self->freeMemory[smallerSize][locationOfLarger*2+1]=FREE;
            locationOfLarger=locationOfLarger*2;
            smallerSize--;
            }
            allocationStart=power(2,subMemoryIndex)*locationOfLarger;
            allocationEnd=power(2,subMemoryIndex)*locationOfLarger+power(2,subMemoryIndex)-1;
            MemoryLocation *address= (MemoryLocation *)malloc(sizeof(MemoryLocation));
            address->id=proccessID;
            address->startAddress=allocationStart;
            address->endAddress=allocationEnd;
            memoryMapInsert(self->map,proccessID,address);
            return address;
        }  
    }
}

void deallocatePrivate( Memory* self,int start,int end)
{
    int usedSize=end-start+1;
    int subMemoryIndex=logarithm2(usedSize);
    int startIndex=start/usedSize;
    //Merging is required
    if(subMemoryIndex!=10&&startIndex%2==0 && self->freeMemory[subMemoryIndex][startIndex+1]==FREE) 
    {
        self->freeMemory[subMemoryIndex][startIndex+1]=NON_EXISTENT;
        deallocatePrivate(self,start,end+usedSize);
    }
    //Merging is also required
    else if((subMemoryIndex!=10)&&(startIndex>0)&&((startIndex-1)%2==0)&&(self->freeMemory[subMemoryIndex][startIndex-1]==FREE) )
    {
        self->freeMemory[subMemoryIndex][startIndex-1]=NON_EXISTENT;
        deallocatePrivate(self,start-usedSize,end);
    }
    else //merging is not required [base case]
        self->freeMemory[subMemoryIndex][startIndex]=FREE;
}

bool deallocate( Memory* self,int processID)
{
    MemoryLocation * address=memoryMapRemove(self->map,processID);
    if(!address) return false;
    int allocationStart=address->startAddress;
    int allocationEnd=address->endAddress;
    free(address);
    deallocatePrivate(self,allocationStart,allocationEnd);
    return true;
}

//This function should be called after allocation and before deallocation
void memoryLog(Memory* self,int processID,int currentTime,bool IamAllocating)
{
    MemoryLocation* address=memoryMapFind(self->map,processID);
    char*I=IamAllocating?" allocated ":" freed ";
    char*allocationMessage,*tempStr1,*tempStr2,*tempStr3,*tempStr4,*tempStr5,*tempStr6;

    char*tempStr12,*tempStr34,*tempStr56,*tempStr1234;
    char currentTimeString[16],sizeString[16],processIDString[16],startAddressString[16],endAddressString[16];
    snprintf(currentTimeString, sizeof(currentTimeString), "%d", currentTime);
    snprintf(sizeString, sizeof(sizeString), "%d", address->endAddress-address->startAddress+1);
    snprintf(processIDString, sizeof(processIDString), "%d", address->id);
    snprintf(startAddressString, sizeof(startAddressString), "%d", address->startAddress);
    snprintf(endAddressString, sizeof(endAddressString), "%d", address->endAddress);

    tempStr1=concat("At time ",currentTimeString);
    tempStr2=concat(I,sizeString);
    tempStr12=concat(tempStr1,tempStr2);free(tempStr1);free(tempStr2);
    tempStr3=concat(" bytes for process ",processIDString);
    tempStr4=concat(" from ",startAddressString);
    tempStr34=concat(tempStr3,tempStr4);free(tempStr3);free(tempStr4);
    tempStr5=concat(" to ",endAddressString);
    tempStr6=concat(" ","\n");
    tempStr56=concat(tempStr5,tempStr6);free(tempStr5);free(tempStr6);
    tempStr1234=concat(tempStr12,tempStr34);free(tempStr12);free(tempStr34);
    allocationMessage=concat(tempStr1234,tempStr56);free(tempStr1234);free(tempStr56);

    fputs(allocationMessage, self->memoryFilePtr);free(allocationMessage);
}

void printAddress(MemoryLocation* address)
{
    if(address)
    printf("allocated memory from %d to %d\n",address->startAddress,address->endAddress);  
    else
    printf("Allocation failed! memory is FULL\n");  
}


