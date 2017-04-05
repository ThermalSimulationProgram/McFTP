#include "Worker.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "Enumerations.h"
#include "Semaphores.h"
#include "TimeUtil.h"
#include "Operators.h"
#include "Statistics.h"
#include "Priorities.h"
#include "Scratch.h"
#include "Pipeline.h"


using namespace std;

#define _INFO 0
#define _DEBUG 0



Worker::Worker(int _stageId, int _id) : Thread(_id), load(Scratch::getBenchmarkName()){
	stageId     = _stageId; 
	next        = NULL;
	thread_type = worker;
	current_job = NULL;
	state 		= _active;

	base        = 200;
	ton         = 100000; // default value, unit us 
	toff        = 0; // default value, unit us 
	
	sem_init(&FIFO_sem, 0, 1);
	sem_init(&schedule_sem, 0, 0);
	sem_init(&ptm_sem, 0, 1);
	sem_init(&state_sem, 0, 1);


	latestSleep = TimeUtil::Millis(0);
}

Worker::~Worker(){
	 
}


bool Worker::isInitialized(){
	return initialized;
}


void Worker::join(){
	sem_post(&FIFO_sem);
	sem_post(&FIFO_sem);
	sem_post(&FIFO_sem);
	sem_post(&schedule_sem);
	sem_post(&schedule_sem);
	sem_post(&ptm_sem);
	sem_post(&ptm_sem);
	sem_post(&state_sem);
	join2();
}

void Worker::getAllInfo(double now, WorkerInfo & ret){
	ret.stageId = stageId;
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
	else
		ret.sleepTime = 0;
	
	ret.allEventAbsDeadlines = getAllAbsDeadline_ms();
	#if _DEBUG==1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " already got its job deadlines\n";
	Semaphores::print_sem.post_sem();
	#endif

	ret.nFIFOJobs            =  (int)ret.allEventAbsDeadlines.size();
	
	if (current_job != NULL){
		ret.onGoEventId = (int)current_job->getId();
		ret.executed    = ((double)current_job->getABET())/1000;
	}
	else{
		ret.onGoEventId = 0;
		ret.executed    = 0;
	}
}

unsigned long Worker::getExecuted(){
	unsigned long ret = 0;
	if (current_job != NULL)
		ret = current_job->getABET();

	return ret;
}

vector<unsigned long> Worker::getAllAbsDeadline(){
	vector<unsigned long> ret;
	sem_wait(&FIFO_sem);
	for (unsigned i = 0; i < FIFO.size(); ++i)
	{
		ret.push_back(FIFO[i]->getAbsDeadline());
	}
	sem_post(&FIFO_sem);
	return ret;
}

vector<double> Worker::getAllAbsDeadline_ms(){
	vector<double> ret;
	sem_wait(&FIFO_sem);
	for (unsigned i = 0; i < FIFO.size(); ++i)
	{
		ret.push_back((double)FIFO[i]->getAbsDeadline()/1000);
	}
	sem_post(&FIFO_sem);
	return ret;
}

void Worker::activate(){
	setPriority(Priorities::get_active_pr());
}

void Worker::wrapper(){

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

  	//Wait until the simulation is initialized
	while( !Pipeline::isInitialized() );
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " waiting for simulation start\n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!Pipeline::isSimulating()){};

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker: " << id << " begining execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	unsigned long exedSlice;
	unsigned long start;
	unsigned long end;
	unsigned long total_exed;
	// unsigned int seed;
	while(Pipeline::isSimulating())
	// while(1)
	{
		if (toff >= 100){
			sem_wait(&state_sem);
		 	latestSleep = TimeUtil::getTime();
			state = _sleep;
			sem_post(&state_sem);
			#if _DEBUG == 1
			Semaphores::print_sem.wait_sem();
			cout << "Worker: " << id << " waiting for sem_ptm\n";
			Semaphores::print_sem.post_sem();
  			#endif
			
			sem_wait(&ptm_sem);
			struct timespec sleepEnd = latestSleep + TimeUtil::Micros(toff);
			sem_post(&ptm_sem);

			// try sleep toff us
			Statistics::addTrace(thread_type, id, sleep_start);
			if (sem_timedwait(&schedule_sem, &sleepEnd) == 0) // unblocked by the schedule signal
			{
				//cout<<"receives a schedule signal, break from sleep\n";
				Statistics::addTrace(thread_type, id, sleep_end);
				continue; // if current job is sleeping when it receives a schedule signal, quit PTM to continue next PTM with new ton and toff
			}
		}
		// sqrt(rand_r(&seed));
		// load.cpu_stressor.stressWithMethod(1);
		if (ton >= 1000)
		{	sem_wait(&state_sem);
			latestSleep = TimeUtil::Millis(0);
			state = _active;
			sem_post(&state_sem);

			//Then be active for ton us, waste base time slice with 100 us length.
			//after consuming 100us, update the execution info if is handling any job.
			start = TimeUtil::convert_us(TimeUtil::getTime());
			total_exed = 0;
			bool stop = false;
			// Statistics::addTrace(thread_type, id, active_start);
			do //ton loop
			{
				if (sem_trywait(&schedule_sem) == 0)//successfully read a schedule singal, break immediately
				{
					//cout<<"receives a schedule signal, break from active\n";
					// Statistics::addTrace(thread_type, id, active_end);
					break;
				}
				// load.consume_us(base);
				// load.consume_us_rand(base);
				load.consume_us_benchmarks(base);
				end = TimeUtil::convert_us(TimeUtil::getTime());
				exedSlice = end - start;
				total_exed = total_exed + exedSlice;
				start = end;
				if (current_job != NULL){
					if (current_job->execute(exedSlice) == 1){
						finishedJob();
					}
				}
				else{
					tryLoadJob();
				}

				sem_wait(&ptm_sem);
				if (total_exed > ton)
					stop = true;
				sem_post(&ptm_sem);
			} while ((!stop) && Pipeline::isSimulating() );	
			Statistics::addTrace(thread_type, id, active_end); 
		}
	}


  #if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Worker " << id << " exiting wrapper...\n";
	Semaphores::print_sem.post_sem();
  #endif
}

// 

void Worker::tryLoadJob(){
	sem_wait(&FIFO_sem);
	if (FIFO.size()>0 && current_job == NULL) 
		current_job = FIFO.front();
	sem_post(&FIFO_sem);
}


void Worker::newJob(Job * j)
{
	sem_wait(&FIFO_sem);
	FIFO.push_back(j);
	sem_post(&FIFO_sem);
	if (j->joinStage(stageId))
	{
		Semaphores::print_sem.wait_sem();
		cout << "Worker " << id << " add new job error...\n";
		Semaphores::print_sem.post_sem();
	}
}


void Worker::finishedJob()
{

	#if _DEBUG == 1
  	Semaphores::print_sem.wait_sem();
  	cout << "Worker: " << id << " is finishing a job: " << current_job->getId() << endl;
  	Semaphores::print_sem.post_sem();
  	#endif
   //	pipeline->finishedJob(current_job);

	if (next != NULL)
		next->newJob(current_job);	
	else{
		pipeline->finishedJob(current_job);
	}
	sem_wait(&FIFO_sem);
    FIFO.pop_front(); 	//Erase old arrival time
    if (FIFO.size()!=0) 
    	current_job = FIFO.front();
    else
    	current_job = NULL;
    
    sem_post(&FIFO_sem);
}


void Worker::setPipeline(Pipeline * p)
{
	pipeline = p;
}


void Worker::setPTM(unsigned long _ton, unsigned long _toff)
{
	if (_ton < 100 && _toff < 100 )
	{
		return;
	}
	sem_wait(&ptm_sem);
	ton = _ton;
	toff = _toff;
	sem_post(&schedule_sem);
	sem_post(&ptm_sem);

}


