#ifndef _STRUCTDEF_H
#define _STRUCTDEF_H

#include <vector>
#include <string>
#include <time.h>


#include "utils/Enumerations.h"


class Task;

typedef struct
{
   int                workerId;
   int                onGoJobId;
   enum _task_type    onGoJobType;
   enum _worker_state state;
   struct timespec    latestExecuteTime; //When the core started to execute the latest job, unit microsecond, 0 means no job
   struct timespec    latestSleepTime;   //the latest time when the core entered sleep state, unit microsecond, 0 means currently active
} CoreInfo;


typedef struct
{
   std::vector <CoreInfo>              coreinfos;
   std::vector <std::vector <Task *> > tasksInQueue;
   std::vector <double>                hardwareTemperature;
   std::vector <double>                softwareTemperature;
   double                              currentTime;
   int                                 numstages;
}DynamicInfo;


typedef struct
{
   std::string counterName;
   double      valueScale;
   double      weight;
}SoftTemperatureSensorConfig;



#endif
