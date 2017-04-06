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


class CMI;

class Task;


class Worker : public Thread
{
protected:
	int workerId;

	enum _worker_state state;
	struct timespec latestSleep;

	///This vector stores the jobs that are waiting for execution
	

	Task* current_job;


	CMI * cmi;

	// ///semaphore controls setting the job queue
	// sem_t queue_sem;

	// //semaphore controls work loads from the job queue
	// sem_t queue_lock_sem;

	///semaphore controls accessing state and latestSleep
	sem_t state_sem;

	struct timespec sleepLength;

	sem_t suspend_sem;

	sem_t resume_sem;

public:
	Worker(int, int);
	~Worker();

	void join();
	
	void activate();

	void deactivate(const struct timespec& length);

	inline void setSuspendPoint(){
    if (sem_trywait(&suspend_sem) == 0)//successfully read a suspend singal, block the executing immediately
    {
      struct timespec now = TimeUtil::getTime();
      struct timespec sleepEnd = now + sleepLength;
      int resumeVal;

      // make sure the resume semaphore is cleared
      sem_getvalue(&resume_sem, &resumeVal);
      for (int i = 0; i < resumeVal; ++i){
         sem_trywait(&resume_sem);
      }
      latestSleep = now;
      state = _sleep;
      
      // two exit conditions: reach the sleepEnd time, or recieves a resume_sem signal
      sem_timedwait(&resume_sem, &sleepEnd);
      
      state = _active;
      latestSleep = TimeUtil::Millis(0);
    }

  };


	void wrapper();

	// void addJob(Task * t);

	// void finishedJob();

	// // lock the job queue. When the worker finishes current job, 
	// // it cannot load new job from the queue
	// void lockQueue();

	// // unlock the job queue
	// void unlockQueue();

	// void setJobQueue(const JobQueue q);

	void setCMI(CMI*);

	void getAllInfo(double, WorkerInfo&);

	unsigned long getExecuted();

	std::vector<unsigned long> getAllAbsDeadline();

	std::vector<double> getAllAbsDeadline_ms();

	bool isActive();

};

#endif