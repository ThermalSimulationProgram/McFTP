#ifndef _TIMEDRUNNABLE_H
#define _TIMEDRUNNABLE_H

#include <vector>
#include <time.h>
#include <semaphore.h>


#include "Thread.h"

class TimedRunnable : public Thread{

protected:

	///This vector stores the absolute times instances when the timedJob functin is called
	// must be set before simulation starts
	std::vector<struct timespec> abs_times;

	///Thie attribute indicates if vector abs_times has been set
	bool abs_times_ready;

	///This semaphore is used to block the thread until next absolute times instance in abs_times
	sem_t wrapper_sem;

	

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

	///This function set the abs_times vector
	void setAbsTimes(const std::vector<struct timespec>&);

	///This fucntion calculates the abs_times vector according to given
	/// relative times and time offset.
	void convertRelativeTimesToAbs(const std::vector<unsigned long>&, unsigned long);

	// pure virtual function. Should be implemented in derived classes
	virtual void timedJob(unsigned) = 0;


};


#endif