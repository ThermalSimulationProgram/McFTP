#include "core/Worker.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "utils/Enumerations.h"
#include "utils/Semaphores.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "results/Statistics.h"
#include "pthread/Priorities.h"
#include "configuration/Scratch.h"
#include "core/CMI.h"
#include "core/Task.h"

using namespace std;

#define _INFO 0
#define _DEBUG 0



Worker::Worker(int _workerId, int _id) : Thread(_id){
	workerId     = _workerId; 
	thread_type = worker;
	current_job = NULL;
	state 		= _active;
	
	// sem_init(&queue_sem, 0, 1);
	// sem_init(&queue_lock_sem, 0, 1);
	// sem_init(&job_sem, 0, 0);
	sem_init(&state_sem, 0, 1);
	sem_init(&suspend_sem, 0, 0);
	sem_init(&resume_sem, 0, 0);


	latestSleep = TimeUtil::Millis(0);
}

Worker::~Worker(){
	 
}




void Worker::join(){
	// sem_post(&queue_sem);
	// sem_post(&queue_sem);
	sem_post(&resume_sem);
	sem_post(&state_sem);
	join2();
}

void Worker::getAllInfo(double now, WorkerInfo & ret){
	ret.stageId = workerId;
	struct timespec tmp;
	
	sem_wait(&state_sem);
	ret.state = state;
	if (state == _sleep)
		tmp = latestSleep;
	sem_post(&state_sem);

	#if _DEBUG==1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " already got its state\n";
	Semaphores::print_sem.post_sem();
	#endif
	
	if (ret.state == _sleep){
		unsigned long rlLatestSleep = TimeUtil::convert_us(
			tmp - Statistics::getStart());
		ret.sleepTime = (now - (((double)rlLatestSleep)/1000));
	}
	else{
		ret.sleepTime = 0;
	}
	
	ret.allEventAbsDeadlines = getAllAbsDeadline_ms();
	#if _DEBUG==1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " already got its job deadlines\n";
	Semaphores::print_sem.post_sem();
	#endif

	ret.nFIFOJobs            =  0;
	
	if (current_job != NULL){
		ret.onGoEventId = (int)current_job->getId();
		ret.executed    = 0;
	}
	else{
		ret.onGoEventId = 0;
		ret.executed    = 0;
	}
}

unsigned long Worker::getExecuted(){
	unsigned long ret = 0;

	return ret;
}

vector<unsigned long> Worker::getAllAbsDeadline(){
	vector<unsigned long> ret;
	// sem_wait(&queue_sem);
	// for (unsigned i = 0; i < FIFO.size(); ++i)
	// {
	// 	ret.push_back(FIFO[i]->getAbsDeadline());
	// }
	// sem_post(&queue_sem);
	return ret;
}


vector<double> Worker::getAllAbsDeadline_ms(){
	vector<double> ret;
	// sem_wait(&queue_sem);
	// //     
	// sem_post(&queue_sem);
	return ret;
}


void Worker::activate(){
	// setPriority(Priorities::get_active_pr());
	sem_post(&resume_sem);
	if (current_job != NULL){
		current_job->resume();
	}
}

void Worker::deactivate(const struct timespec& length){
	// setPriority(Priorities::get_inactive_pr());
	sleepLength = length;
	sem_post(&suspend_sem);
	if (current_job != NULL){
		current_job->suspend(length);
	}
}


bool Worker::isActive(){
	bool ret;
	sem_wait(&state_sem);
	if (state == _sleep){
		ret = false;
	}else{
		ret = true;
	}
	sem_post(&state_sem);
	return ret;
}

// lock the job queue. When the worker finishes current job, 
// it cannot load new job from the queue
// void Worker::lockQueue(){
// 	sem_wait(&queue_lock_sem);
// }

// 	// unlock the job queue
// void Worker::unlockQueue(){
// 	sem_post(&queue_lock_sem);
// }


// void Worker::setJobQueue(const JobQueue& q){
// 	sem_wait(&queue_sem);
// 	// sem_init(&job_sem, 0, 0);
// 	queue = q;
// 	// for (int i = 0; i < queue.getSize(); ++i)
// 	// {
// 	// 	sem_post(&job_sem);
// 	// }
// 	sem_post(&queue_sem);
// }

void Worker::wrapper(){

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

  	//Wait until the simulation is initialized
	while( !CMI::isInitialized() ){}
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for simulation start\n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!CMI::isRunning()){}

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " begining execution \n";
	Semaphores::print_sem.post_sem();
  	#endif
	
	while(CMI::isRunning())
	{
		setSuspendPoint();

		if (current_job == NULL){
			// sem_wait(&job_sem);
			current_job = cmi->tryLoadJob(workerId);
		}else{
			current_job->setWorker(this);
			current_job->fire();
			setSuspendPoint();
			cmi->finishedJob(current_job);
			current_job = NULL;
		}
	}
}


// void Worker::tryLoadJob(){
// 	sem_wait(&queue_lock_sem); 
// 	sem_wait(&queue_sem);
// 	current_job = queue.pop_front();
// 	sem_post(&queue_sem);
// 	sem_post(&queue_lock_sem);
// }


// void Worker::addJob(Task* newTask){
// 	sem_wait(&queue_sem);
// 	queue->insertJob(newTask);
// 	sem_post(&job_sem);
// 	sem_post(&queue_sem);
// }

void Worker::setCMI(CMI * c)
{
	cmi = c;
}




