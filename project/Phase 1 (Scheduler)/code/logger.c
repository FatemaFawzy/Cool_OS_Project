#include "headers.h"

typedef struct{
    float sumWaiting;
    float sumWTA;
    float listWTA[MAX_SIZE];
    int listSize;
    FILE *logFilePtr;
    FILE *perfFilePtr;
}Logger;

// Conor (without allocation)
void Logger__init( Logger* self) {
    self->listSize=0;
    self->sumWaiting=0;
    self->sumWTA=0;
    self->logFilePtr = fopen("Scheduler.log", "w");
    self->perfFilePtr = fopen("Scheduler.perf", "w");
 }

// Allocation + initialization (equivalent to "new Logger(x, y)")
 Logger* createLogger(){
    Logger* result = ( Logger*) malloc(sizeof( Logger));
    Logger__init(result);
    return result;
}

// Deor (without deallocation)
void resetLogger( Logger* self) {
    //Close the files
    fclose(self->logFilePtr);
    fclose(self->perfFilePtr);
}

// Deor + deallocation (equivalent to "delete point")
void destroyLogger( Logger* logger) {
  if (logger) {
     resetLogger(logger);
     free(logger);
  }
}

float computeTA(int arrivalTime,int finishTime)
{
    return finishTime-arrivalTime;
}
float computeWTA(float TA,int burstTime)
{
    return TA/burstTime;
}

float computeStdWTA(Logger* self,float avgWTA)
{
    float std=0.0;
    for(int i=0;i<self->listSize;i++)
        std+=(self->listWTA[i]-avgWTA)*(self->listWTA[i]-avgWTA);
    return std/self->listSize;
}

void schedulerLog(Logger* self,int time,int processID,
ProcessState state,int arr,int total,int remain,int wait)
{
    printf("Entered Logger\n");
    char*logMessage,*tempStr1,*tempStr2,*tempStr3,*tempStr4,*tempStr5,*tempStr6,*tempStr7,*tempStr8,*tempStr9,*tempStr10;
    char*tempStr12,*tempStr34,*tempStr56,*tempStr78,*tempStr910,*tempStr1234,*tempStr5678,*tempStr12345678;
    char currentTimeString[16],processIDString[16];
    char arrString[16],totalString[16],remainString[16],waitString[16],TAString[16],WTAString[16];

    snprintf(currentTimeString, sizeof(currentTimeString), "%d", time);
    snprintf(processIDString, sizeof(processIDString), "%d", processID);
    snprintf(arrString, sizeof(arrString), "%d", arr);
    snprintf(totalString, sizeof(totalString), "%d", total);
    snprintf(remainString, sizeof(remainString), "%d", remain);
    snprintf(waitString, sizeof(waitString), "%d", wait);

    char*stateString;

    switch (state)
    {
    case STARTED:
        stateString="started";
        break;
    case RESUMED:
        stateString="resumed";
        break;
    case STOPPED:
        stateString="stopped";
        break;
    case FINISHED:
        stateString="finished";
        break;
    default:
        break;
    }
    
    tempStr1=concat("At time ",currentTimeString);
    tempStr2=concat(" process ",processIDString);
    tempStr12=concat(tempStr1,tempStr2);free(tempStr1);free(tempStr2);
    tempStr3=concat(" ",stateString);
    tempStr4=concat(" arr ",arrString);
    tempStr34=concat(tempStr3,tempStr4);free(tempStr3);free(tempStr4);
    tempStr5=concat(" total ",totalString);
    tempStr6=concat(" remain ", remainString);
    tempStr56=concat(tempStr5,tempStr6);free(tempStr5);free(tempStr6);
    tempStr7=concat(" wait ", waitString);

    float TA,WTA;
    if(state==FINISHED)
    {
        TA=computeTA(arr,time);
        WTA=computeWTA(TA,total);
        snprintf(TAString, sizeof(TAString), "%.2f", TA);
        snprintf(WTAString, sizeof(WTAString), "%.2f", WTA);
        tempStr8=concat(" TA ", TAString);
        tempStr9=concat(" WTA ", WTAString);
        tempStr10=concat(" ", "\n");
        tempStr910=concat(tempStr9,tempStr10);free(tempStr9);free(tempStr10);
    }
    else
    {
        tempStr8=concat(" ", "\n");
    }
    tempStr78=concat(tempStr7,tempStr8);free(tempStr7);free(tempStr8);
    tempStr1234=concat(tempStr12,tempStr34);free(tempStr12);free(tempStr34);
    tempStr5678=concat(tempStr56,tempStr78);free(tempStr56);free(tempStr78);
    if(state==FINISHED)
    {
        tempStr12345678=concat(tempStr1234,tempStr5678);free(tempStr1234);free(tempStr5678);
        logMessage=concat(tempStr12345678,tempStr910);free(tempStr12345678);free(tempStr910);

        //Add TA and WTA to the sum, and Store WTA for std
        self->listWTA[self->listSize++]=WTA;
        self->sumWaiting+=wait;
        self->sumWTA+=WTA;
    }  
    else
    {
        logMessage=concat(tempStr1234,tempStr5678);free(tempStr1234);free(tempStr5678);
    }
    printf("Supposedly logged %s\n",logMessage);
    fputs(logMessage, self->logFilePtr);free(logMessage);
    
}

void schedulerPerf(Logger* self)
{
    float avgWTA=self->sumWTA/self->listSize;
    float avgWaiting=self->sumWaiting/self->listSize;
    float stdWTA=computeStdWTA(self,avgWTA);

    char avgWTAString[16],avgWaitingString[16],stdWTAString[16];
    snprintf(avgWTAString, sizeof(avgWTAString), "%.2f", avgWTA);
    snprintf(avgWaitingString, sizeof(avgWaitingString), "%.2f", avgWaiting);
    snprintf(stdWTAString, sizeof(stdWTAString), "%.2f", stdWTA);

    char* avgWTAMessage=concat("Avg WTA = ",avgWTAString);
    char* avgWaitingMessage=concat("Avg Waiting = ",avgWaitingString);
    char* stdWTAMessage=concat("Std WTA = ",stdWTAString);

    strcat(avgWTAMessage,"\n");
    strcat(avgWaitingMessage,"\n");
    strcat(stdWTAMessage,"\n");

    fputs(avgWTAMessage, self->perfFilePtr );free(avgWTAMessage);
    fputs(avgWaitingMessage, self->perfFilePtr );free(avgWaitingMessage);
    fputs(stdWTAMessage, self->perfFilePtr);free(stdWTAMessage);
}