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
#include "core/Processor.h"
#include "core/Task.h"

using namespace std;

#define _INFO 0
#define _DEBUG 0



Worker::Worker(int _workerId, int _id) : Thread(_id){
	workerId    = _workerId; 
	thread_type = _worker;
	current_job = NULL;
	state 		= _active;
	
	sem_init(&job_sem, 0, 1);
	sem_init(&state_sem, 0, 1);
	sem_init(&suspend_sem, 0, 0);
	sem_init(&resume_sem, 0, 0);
	sem_init(&exetime_sem, 0, 1);


	latestSleep = TimeUtil::Millis(0);
	latestExecuteJob = latestSleep;

}

Worker::~Worker(){
	 
}

void Worker::join(){
	sem_post(&resume_sem);
	sem_post(&state_sem);
	sem_post(&state_sem);
	sem_post(&job_sem);
	join2();
}


void Worker::getCoreInfo(CoreInfo& cinfo){
	cinfo.workerId = workerId;
	sem_wait(&state_sem);
	cinfo.state = state;
	cinfo.latestSleepTime = latestSleep;
	sem_post(&state_sem);

	cinfo.latestExecuteTime = getlatestExecuteJobTime();

	sem_wait(&job_sem);
	if (current_job!=NULL){
		cinfo.onGoJobId = current_job->getId();
		cinfo.onGoJobType = current_job->getType();
	}else{
		cinfo.onGoJobId = 0;
		cinfo.onGoJobType = singlecore;
	}
	sem_post(&job_sem);
}

Task * Worker::stopCurrentJob(){
	Task * ret = NULL;
	sem_wait(&job_sem);
	if (current_job != NULL){
		current_job->resume();
		ret = current_job;
	}
	sem_post(&job_sem);
	return ret;
}

void Worker::activate(){
	// setPriority(Priorities::get_active_pr());
	sem_wait(&state_sem);
	state = _active;
	latestSleep = TimeUtil::Millis(0);
	sem_post(&state_sem);
	sem_post(&resume_sem);
	
	sem_wait(&job_sem);
	if (current_job != NULL){
		current_job->resume();
	}
	sem_post(&job_sem);

}

void Worker::deactivate(const struct timespec& length){

	struct timespec now = TimeUtil::getTime();
    sleepEnd = now + length;

    sem_wait(&state_sem);
    latestSleep = now;
    state = _sleep;
    sem_post(&state_sem);

	sem_post(&suspend_sem);
	sem_wait(&job_sem);
	if (current_job != NULL){
		current_job->suspend(sleepEnd);
	}
	sem_post(&job_sem);
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


void Worker::lockCurrentJob(){
	sem_wait(&job_sem);
}

void Worker::unlockCurrentJob(){
	sem_post(&job_sem);
}

void Worker::wrapper(){

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

  	//Wait until the simulation is initialized
	sem_wait(&Processor::init_sem);

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for simulation start\n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	sem_wait(&Processor::running_sem);

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " begining execution \n";
	Semaphores::print_sem.post_sem();
  	#endif
	
	while(Processor::isRunning())
	{
		setSuspendPoint();

		if (current_job == NULL){
			sem_wait(&job_sem);
			current_job = processor->tryLoadJob(workerId);
			sem_post(&job_sem);
		}else{
			current_job->setWorker(this);
			
			sem_wait(&exetime_sem);
			latestExecuteJob = TimeUtil::getTime();
			sem_post(&exetime_sem);

			current_job->fire();			
			processor->finishedJob(current_job);

			sem_wait(&job_sem);
			current_job = NULL;
			sem_post(&job_sem);
		}
	}

	
}

void Worker::setProcessor(Processor * c){
	processor = c;
}
int Worker::getWorkerId(){
	return workerId;
}


struct timespec Worker::getLatestSleepTime(){
	struct timespec ret;
	sem_wait(&state_sem);
	ret = latestSleep;
	sem_wait(&state_sem);
	return ret;
}

struct timespec Worker::getlatestExecuteJobTime(){
	struct timespec ret;
	sem_wait(&exetime_sem);
	ret = latestExecuteJob;
	sem_wait(&exetime_sem);
	return ret;
}

