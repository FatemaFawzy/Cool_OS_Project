#include "headers.h"
#include "logger.c"

int main(int argc, char * argv[])
{   
    Logger *logger=createLogger();

    schedulerLog(logger,1,11,STARTED,1,2,3,3);
    schedulerLog(logger,2,10,STARTED,1,2,3,3);
    schedulerLog(logger,3,10,STOPPED,1,2,3,3);
    schedulerLog(logger,4,10,FINISHED,1,2,3,3);
    schedulerLog(logger,4,11,FINISHED,1,1,3,4);

    //This should be called after all the processes have finished
    schedulerPerf(logger);
    destroyLogger(logger);
}