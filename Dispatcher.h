#ifndef _DISPATCHER_H
#define _DISPATCHER_H

#include <vector>
#include <time.h>


#include "TimedRunnable.h"


class CMI;
class Job;


class Dispatcher : public TimedRunnable
{
private:
	//This vector stores the relative release times (in millisecond) of the jobs, loaded from input csv file
	//relative release time is the time from the beginning  of the simulation, 
	std::vector<unsigned long> rl_arrive_times;

	std::vector<struct timespec> abs_arrive_times;

	//This vector stores all the job objects
	std::vector<Job> jobs;

	///This pointer points to the Pipeline object to which this dispatcher is linked
	CMI* cmi;


public:
	///Constructor needs the relative release times of the jobs
	Dispatcher(Pipeline* ,const std::vector<unsigned long>&, unsigned);
	~Dispatcher();

	///gives the dispatcher thread the ACTIVE_PR priority
	void activate();

	///This join function takes into account the dispatcher's unblocking mechanism
	void join();

	///This function statically creates  all the jobs according to given WCETs,
	/// execution time variation factor, and the relative deadline of the task. 
	void createJobs(std::vector<unsigned long>, float, unsigned long);

	///The created thread in the object starts execution by invoking this function 
	void wrapper();

	/// This function is invoked at given time instances to release a new job.
	void timedJob(unsigned);

	///This function calculates the absolute release time instance for all job, 
	/// must be called before actual dispatching 
	void setAbsReleaseTimes(unsigned long);

	///This function links the dispatcher to a Pipeline object
	// void setPipeline(Pipeline*);

};

#endif