#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <vector>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// #include "Pipeline.h"
#include "Thread.h"
#include "Enumerations.h"
#include "TimeUtil.h"
#include "TimedRunnable.h"

#include "structdef.h"
#include "ScheduleKernelEnumerations.h"


class ScheduleKernelAPI;
class Pipeline;

class Scheduler : public TimedRunnable
{
protected:

	/////////////////////////////////////////////
	std::vector<unsigned long> rl_scheduling_times;
	
	Pipeline * pipeline;

	enum _schedule_kernel kernel_type;
	
	ScheduleKernelAPI *kernelAPI;

public:
	Scheduler(Pipeline *, _schedule_kernel,  unsigned);
	
	~Scheduler();

	void init();
	// bool isInitialized();
	void activate();

	void wrapper();

	void setAbsSchedulingTimes(unsigned long);
	void join();

	void timedJob(unsigned);

	// void getPipelineInfo(pipeinfo&, const vector<double>&, const vector<double>&);

	std::vector<double> getKernelTime();

	Pipeline* getPipelinePointer();

	std::vector<std::vector<double> > getAllSchemes();

};






#endif