#ifndef _WORKER_H
#define _WORKER_H


#include "Load.h"
#include "Job.h"
#include "Thread.h"
#include "structdef.h"


#include <vector>
#include <deque>
#include <time.h>
#include <semaphore.h>
// #include <pthread.h>



class Pipeline;



class Worker : public Thread
{
protected:
	int stageId;
	unsigned long ton;
	unsigned long toff;

	enum _worker_state state;
	struct timespec latestSleep;


	///This vector stores the ids of jobs that are waiting for execution
	std::deque<Job *> FIFO;
	Job* current_job;

	unsigned base ;


	Pipeline * pipeline;

	///This attribute points to next stage, should be NULL when it's the end stage
	Worker *next;

	///semaphore controls accessing vector FIFO
	sem_t FIFO_sem;

	///semaphore controls accessing ton and toff
	sem_t ptm_sem;

	///semaphore controls accessing state and latestSleep
	sem_t state_sem;

	///signal semaphore, unblock worker from sleeping and running 
	sem_t schedule_sem;

	Load load;

public:
	Worker(int, int);
	~Worker();

	void join();
	
	void activate();

	void wrapper();

	void newJob(Job*);

	void tryLoadJob();

	void finishedJob();

	void setNext(Worker*);

	void setPipeline(Pipeline*);

	void setPTM(unsigned long, unsigned long);


	void getAllInfo(double, WorkerInfo&);

	unsigned long getExecuted();

	std::vector<unsigned long> getAllAbsDeadline();

	std::vector<double> getAllAbsDeadline_ms();

	bool isInitialized();

};

#endif