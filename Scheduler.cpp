#include "Scheduler.h"

#include <iostream>

#include "Priorities.h"
#include "Semaphores.h"
#include "Scratch.h"
#include "Pipeline.h"
#include "ScheduleKernelAPI.h"

using namespace std;

#define _INFO 0
#define _DEBUG 0;

Scheduler::Scheduler(Pipeline * p, _schedule_kernel _sche_type, \
	 unsigned _id): TimedRunnable(_id){
	///  initialize member vairables
	pipeline                      = p;
	thread_type                   = scheduler;
	kernel_type                   = _sche_type;
	kernelAPI 					  = NULL;
	
	/// set relative scheduling time instances
	unsigned long adaption_period = Scratch::getAdaptionPeriod();
	int count                     = 1;
	if (adaption_period != 0 ){
		unsigned long sim_length = Scratch::getDuration();
		count                    = sim_length/adaption_period;
		count                    = count == 0? 1 : count; // at least one adaption at beginning
	}
	
	for (int i = 0; i < count; ++i){
		rl_scheduling_times.push_back((unsigned long) i*adaption_period);	
	}
}

Scheduler::~Scheduler()
{
	delete kernelAPI;
}

// note: since this function creates a Java Virtual Machine,
// this function can only be called by the thread which will use the JVM in future,
// i.e., the thread created in this class 
void Scheduler::init(){
	kernelAPI = new ScheduleKernelAPI(this, kernel_type, rl_scheduling_times);
	// all initialized, post the semaphore to signal main thread scheduler is ready
	Semaphores::rtcinit_sem.post_sem();
}


void Scheduler::activate(){
	setPriority(Priorities::get_sched_pr(3));
}


void Scheduler::join(){
	join2();
}

void Scheduler::wrapper(){
	init();
	#if _INFO == 1
  	Semaphores::print_sem.wait_sem();
	cout << "Scheduler: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

	
  	//Wait until the simulation is initialized
	while( !Pipeline::isInitialized() );

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Scheduler: " << id << " wating for execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!Pipeline::isSimulating()){};


	// this function is inherited from TimedRunable class
	timedRun();

	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "Scheduler " << id << " exiting wrapper...\n";
	Semaphores::print_sem.post_sem();
  	#endif
}

void Scheduler::setAbsSchedulingTimes(unsigned long start_time){
	convertRelativeTimesToAbs(rl_scheduling_times, start_time);
}

void Scheduler::timedJob(unsigned _index){
	unsigned n = Scratch::getNstage() ;
	vector<unsigned long> ton, toff;
	vector<double> tons2;
	vector<double> toffs2;
	// cout << "Inside scheduler: going to run kernel\n";

	kernelAPI->runKernel(tons2, toffs2);

	if (tons2.size()!=n || toffs2.size()!= n){
		cerr << "Scheduler::timedJob:wrong scheduling scheme from the kernel " << endl;
		exit(2);
	}
	for (unsigned i = 0; i < n; ++i){
		ton.push_back(  (unsigned long)(tons2[i]*1000));
		toff.push_back(  (unsigned long)(toffs2[i]*1000));
	}

	pipeline->setPTMs(ton, toff);

}


/// Just an interface function needed by APTM and BWS kernel
// void Scheduler::getPipelineInfo(pipeinfo& config,
// const vector<double>& wcets, const vector<double>& TBET){

// 	pipeline->getInfo(config, wcets, TBET, kernel_type);
	
// }
	

vector<double> Scheduler::getKernelTime(){
	vector<double> ret;
	ret.push_back(kernelAPI->getMaxTimeExpense());
	ret.push_back(kernelAPI->getMeanTimeExpense());
	return ret;
}


Pipeline* Scheduler::getPipelinePointer(){
	return pipeline;
}

std::vector<std::vector<double> > Scheduler::getAllSchemes(){
	return kernelAPI->getAllSchemes();
}

