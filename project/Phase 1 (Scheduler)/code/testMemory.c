#include "headers.h"
#include "memory.c"

int main(int argc, char * argv[])
{   
    Memory *memory=createMemory();
    MemoryLocation* address;

    address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

        address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

        address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

        address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

        address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

        address=allocate(memory,10,200);
    memoryLog(memory,address->id,1,true);
    printAddress(address);
    memoryLog(memory,address->id,1,false);
    deallocate(memory,10);

    // address=allocate(memory,10,25);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);
    // address=allocate(memory,11,100);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);
    // address=allocate(memory,12,17);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);
    // address=allocate(memory,13,66);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);

    // memoryLog(memory,10,1,false);
    // deallocate(memory,10);

    // memoryLog(memory,11,1,false);
    // deallocate(memory,11);

    // address=allocate(memory,14,512);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);

    // memoryLog(memory,12,1,false);
    // deallocate(memory,12);

    // address=allocate(memory,15,33);
    // memoryLog(memory,address->id,1,true);
    // printAddress(address);

    destroyMemory(memory);
}