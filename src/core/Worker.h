#ifndef _WORKER_H
#define _WORKER_H


#include <vector>
#include <deque>
#include <time.h>
#include <semaphore.h>


#include "configuration/JobQueue.h"
#include "pthread/Thread.h"
#include "core/structdef.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"


class Processor;

class Task;


class Worker : public Thread
{
protected:
	int workerId;

	enum _worker_state state;

	struct timespec latestSleep;

	struct timespec latestExecuteJob;

	Task* current_job;

	Processor * processor;

	struct timespec sleepEnd;


	///semaphore controls accessing state and latestSleep
	sem_t state_sem;

	sem_t exetime_sem;

	sem_t suspend_sem;

	sem_t resume_sem;

	sem_t job_sem;

	sem_t stop_sem;

public:
	Worker(int, int);
	~Worker();

	void join();

	Task* stopCurrentJob();
	
	void activate();

	void deactivate(const struct timespec& length);

	inline void setSuspendPoint(){
    if (sem_trywait(&suspend_sem) == 0)//successfully read a suspend singal, block the executing immediately
    {
      int resumeVal;

      // make sure the resume semaphore is cleared
      sem_getvalue(&resume_sem, &resumeVal);
      for (int i = 0; i < resumeVal; ++i){
         sem_trywait(&resume_sem);
      }
      
      // two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
      sem_timedwait(&resume_sem, &sleepEnd);
    }

  };


	void wrapper();

	// lock current job. When the worker finishes current job, 
	// it cannot load new job
	void lockCurrentJob();

	// unlock the job queue
	void unlockCurrentJob();

	void setProcessor(Processor*);

	void getCoreInfo(CoreInfo& cinfo);

	struct timespec getLatestSleepTime();

	struct timespec getlatestExecuteJobTime();

	bool isActive();

	int getWorkerId();


};

#endif