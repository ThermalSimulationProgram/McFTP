#include "ThermalApproach.h"

#include <iostream>

#include "pthread/Priorities.h"
#include "utils/Semaphores.h"
#include "configuration/Scratch.h"
#include "core/CMI.h"
#include "ThermalApproachAPI/ThermalApproachAPI.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;

#define _INFO 0
#define _DEBUG 0;

ThermalApproach::ThermalApproach(unsigned _id, CMI * c, 
	string _approachName): TimedRunnable(_id){
	///  initialize member vairables
	cmi          = c;
	thread_type  = thermal_approach;
	approachName = _approachName;
	
	/// set thermal approach period
	period = Scratch::getAdaptionPeriod();

	timeExpense.reserve(1000);
}

ThermalApproach::~ThermalApproach()
{
	
}

 
void ThermalApproach::init(){
	ThermalApproachAPI::init(approachName);
	// all initialized, post the semaphore to signal main thread scheduler is ready
	Semaphores::rtcinit_sem.post_sem();
}


void ThermalApproach::activate(){
	setPriority(Priorities::get_sched_pr(3));
}


void ThermalApproach::join(){
	join2();
}

void ThermalApproach::wrapper(){
	init();
	struct timespec rem;
	#if _INFO == 1
  	Semaphores::print_sem.wait_sem();
	cout << "ThermalApproach: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

	
  	//Wait until the simulation is initialized
	while( !CMI::isInitialized() );

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "ThermalApproach: " << id << " wating for execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!CMI::isRunning()){}

	while(CMI::isRunning()){

		dynamicinfo d = cmi->getMcFTPInfo();

		struct timespec timein = TimeUtil::getTime();
		configuration c = ThermalApproachAPI::runThermalApproach(d);
		struct timespec timeout = TimeUtil::getTime();

		cmi->updateConfiguration(c);

		timeExpense.push_back(TimeUtil::convert_us(timeout - timein));

		nanosleep(&period, &rem);

	}


	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "ThermalApproach " << id << " exiting wrapper...\n";
	Semaphores::print_sem.post_sem();
  	#endif
}


vector<double> ThermalApproach::getKernelTime(){

	return timeExpense;
}


