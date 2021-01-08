#include "hash.c"


int main()
{
    struct Hash* myTable= createHash(10);

    processData process1={10,9,10,10};
    processData process2={5,5,5,5};
    processData process3={3,3,3,3};
    processData process4={11,11,11,11};
    bool state;
    state = hashInsert(myTable,process1.id,&process1);
    state = hashInsert(myTable,process2.id,&process2);
    state = hashInsert(myTable,process3.id,&process3);
    state = hashInsert(myTable,process4.id,&process4);

    processData* myP = hashFind(myTable,process3.id);
    myP = hashRemove(myTable,process1.id);
    myP = hashFind(myTable,process4.id);

    int x = size(myTable);
    return 0;
}