#ifndef _TIMEDRUNNABLE_H
#define _TIMEDRUNNABLE_H

#include <vector>
#include <time.h>
#include <semaphore.h>


#include "Thread.h"

class TimedRunnable : public Thread{

protected:

	///This variable stores the absolute times instances when the next call of timedJob functin
	struct timespec abs_time;

	bool abs_times_ready;

	///This semaphore is used to block the thread until next absolute times instance in abs_times
	sem_t wrapper_sem;

	///This is a signal semaphore, indicating whether the abs_time is already set
	sem_t time_sem;

public:
	///Constructor needs nothing
	explicit TimedRunnable(unsigned);

	virtual ~TimedRunnable();

	///gives the  thread the ACTIVE priority
	virtual void activate() = 0;

	///The created thread in the object starts execution by invoking this function 
	virtual void wrapper();

	///The function is executed by the thread. It calls timedJob function according to abs_times.
	void timedRun();

	virtual void join() = 0;

	void setAbsTime(struct timespec t);

	// pure virtual function. Should be implemented in derived classes
	virtual void timedJob(unsigned) = 0;


};


#endif