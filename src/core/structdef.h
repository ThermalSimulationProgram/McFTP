#ifndef _STRUCTDEF_H
#define _STRUCTDEF_H 

#include <vector>
#include <string>
#include <time.h>

#include "utils/Enumerations.h"


typedef struct{
	int workerId;
	int onGoJobId;
	enum _task_type onGoJobType;
	enum _worker_state state;
	struct timespec latestExecuteTime; //When the core started to execute the latest job, unit microsecond, 0 means no job
	struct timespec latestSleepTime; //the latest time when the core entered sleep state, unit microsecond, o means currently active
} CoreInfo;


typedef struct{
	std::vector<CoreInfo> coreinfos;
	std::vector<double> temperature;
	double currentTime;
	int numstages;
}DynamicInfo;




#endif


