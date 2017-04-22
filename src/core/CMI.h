#ifndef CMI_H
#define CMI_H 

#include <vector>
#include <string>
#include <semaphore.h>


#include "core/structdef.h"
#include "configuration/Configuration.h"



typedef void (* online_thermal_approach) (const DynamicInfo& p, Configuration& c);
typedef void (* offline_thermal_approach) (Configuration& c);


typedef struct {
	const std::string  name;	// human readable form of approach function 
	const online_thermal_approach func;	/* the thermal online function */
} online_thermal_approach_info_t;

typedef int (* online_task_allocator)(int _taskId);


class Processor;

class CMI{
private:
	int workerNumber;

	// pointer to the processor object, containing all necessary entities
	Processor* processor;

	// The offline approach will be called only once at the beginning of the experiment
	offline_thermal_approach offlineApproach;

	// collection of thermal approaches
	std::vector<online_thermal_approach_info_t> onlineApproaches;

	// the used thermal approach, should be one element of onlineApproaches
	online_thermal_approach validApproach;

	// the index of validApproach in vector onlineApproaches
	int onlineApproachId;

	// function pointer to the function allocating new Jobs.
	// The function will ba called every time a new job is created.
	online_task_allocator taskAllocator;

	// This is the default task allocator
	// syntax: staticTaskAllocator[TASKID] returns the id of the worker on which 
	// the new created job whose taskid is TASKID.
	// users can set it before running the experiment
	std::vector<int> staticTaskAllocator;

public:

	CMI(std::string);

	~CMI();

	void setOfflineThermalApproach(offline_thermal_approach _approach);

	void addOnlineThermalApproach(std::string _name, online_thermal_approach _approach);

	void setValidOnlineThermalApproach(std::string _name);

	void setOnlineThermalApproachPeriod(unsigned long period_us);

	void setTaskRunningCore(int _taskId, int _coreId);

	void setOnlineTaskAllocator(online_task_allocator _allocator);

	std::vector<int> getAllTaskIds();

	void startRunning();

	void printAllTaskInfo();

	unsigned long getRelativeTimeUsec();

	// This function is called by the dispatcher to release a new job
	int getNewJobTargetCore(Task *, _task_type);

	// This function is called by the work 
	void finishedJob(Task*);

	// This function collects all the information of the processor
	// in the simulation
	void getDynamicInfo(DynamicInfo& p);

	unsigned long getCurrentSimTime_ms();

	void lockAllJobs();

	void unlockAllJobs();
};













#endif