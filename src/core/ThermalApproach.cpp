#include "ThermalApproach.h"

#include <iostream>

#include "pthread/Priorities.h"
#include "utils/Semaphores.h"
#include "configuration/Scratch.h"
#include "configuration/Configuration.h"
#include "core/CMI.h"
#include "core/structdef.h"
#include "ThermalApproachAPI/ThermalApproachAPI.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"

using namespace std;

#define _INFO 0
#define _DEBUG 0;

ThermalApproach::ThermalApproach(unsigned _id, CMI * c, 
	string _approachName): Thread(_id){
	///  initialize member vairables
	cmi          = c;
	thread_type  = _thermal_approach;
	approachName = _approachName;
	
	/// set thermal approach period
	period = TimeUtil::Micros(Scratch::getAdaptionPeriod());

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
	setPriority(Priorities::get_tempwatcher_pr());
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
  	sem_wait(&CMI::init_sem);
	// while( !CMI::isInitialized() );

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "ThermalApproach: " << id << " wating for execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	sem_wait(&CMI::running_sem);
	isStatic = Scratch::isStaticApproach();
	// while(!CMI::isRunning()){}

		#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "ThermalApproach: " << id << " begin execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

  	bool isUpdate = true;

	while(CMI::isRunning()){

		DynamicInfo d;
		cmi->getDynamicInfo(d);

		struct timespec timein = TimeUtil::getTime();
		Configuration config = ThermalApproachAPI::runThermalApproach(d);
		struct timespec timeout = TimeUtil::getTime();

		if (isUpdate){
			cmi->updateConfiguration(config);
			if (isStatic){
				isUpdate = false;
			}
		}
		

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


