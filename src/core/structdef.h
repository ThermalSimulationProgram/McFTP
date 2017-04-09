#ifndef _STRUCTDEF_H
#define _STRUCTDEF_H 

#include <vector>
#include <string>
#include <time.h>

#include "utils/Enumerations.h"


typedef struct pipeinfo
{
	std::vector<int>   Q;
	std::vector<int> activeSet;
	std::vector<int> sleepSet;
	std::vector<double> ccs;
	std::vector<double> dcs;
	std::vector<double> rho;
	std::vector<double> K;
	std::vector<std::vector<double> > FIFOcurveData;
	std::vector<double> allT;
	unsigned adaptionIndex;
} pipeinfo;

void pipeinfo_print(const pipeinfo &);

typedef struct ptmspec{
	std::vector<double> tons;
	std::vector<double> toffs;
}ptmspec;


typedef struct taskdata
{	
	int taskId;
	struct timespec period;
	struct timespec jitter;
	struct timespec release_time;
	std::string name;
	std::string benchmark;
	std::vector<int> attached_cores;
	std::vector<struct timespec> wcets;
	std::vector<unsigned long> wcets_us;
}task_data;




typedef struct{
	int stageId;
	int nFIFOJobs;
	double executed;
	enum _worker_state state;
	double sleepTime;
	int onGoEventId;
	std::vector<double> allEventAbsDeadlines;
} WorkerInfo;


typedef struct{
	std::vector<WorkerInfo> workerinfos;
	std::vector<double> temperature;
	double currentTime;
	int numstages;
}DynamicInfo;




#endif


