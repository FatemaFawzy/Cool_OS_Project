#include "headers.h"
#include "memory.c"

void printAddress(MemoryLocation* address)
{
    if(address)
    printf("allocated memory from %d to %d\n",address->startAddress,address->endAddress);  
    else
    printf("Allocation failed! memory is FULL\n");  
}


int main(int argc, char * argv[])
{
    Memory *memory=createMemory();
    MemoryLocation* address;

    address=allocate(memory,10,256);
    printAddress(address);
    address=allocate(memory,11,256);
    printAddress(address);
    address=allocate(memory,12,256);
    printAddress(address);
    address=allocate(memory,13,256);
    printAddress(address);

    deallocate(memory,10);
    deallocate(memory,11);
    address=allocate(memory,14,512);
    printAddress(address);
    deallocate(memory,12);
    address=allocate(memory,15,33);
    printAddress(address);

    address=allocate(memory,16,400);
    printAddress(address);
}