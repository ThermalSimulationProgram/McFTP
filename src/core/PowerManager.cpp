#include "PowerManager.h"

#include <cstdlib>
#include <iostream>
#include <sstream>


#include "core/CMI.h"
#include "configuration/Scratch.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "utils/Semaphores.h"

using namespace std;

#define EPSILON 0.00001
PowerManager::PowerManager(unsigned _id, std::vector<Worker* > _workers):TimedRunnable(_id), 
workers(_workers){

	sem_init(&interrupt_sem, 0, 0);
	sem_init(&statetable_sem, 0, 1);

	struct timespec duration = TimeUtil::Micros(Scratch::getDuration());
	nextActionTime = TimeUtil::getTime() + duration;


	freqInterface.push_back("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu2/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu3/cpufreq/scaling_setspeed");

}

void PowerManager::join(){
	join_unblock();
	join2();
}


void PowerManager::activate(){
	setPriority(Priorities::get_powermanager_pr());
}


void PowerManager::wrapper(){
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "PowerManager: " << id << " waiting for initialization\n";
	Semaphores::print_sem.post_sem();
  	#endif

  	//Wait until the simulation is initialized
	while( !CMI::isInitialized() ){}
	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "PowerManager: " << id << " waiting for simulation start\n";
	Semaphores::print_sem.post_sem();
  	#endif

	///wait for the simulation start
	while(!CMI::isRunning()){}

  	#if _INFO == 1
	Semaphores::print_sem.wait_sem();
	cout << "PowerManager: " << id << " begining execution \n";
	Semaphores::print_sem.post_sem();
  	#endif

	
	while(CMI::isRunning())
	{
		// wait until next action time, or new tables are given
		sem_timedwait(&interrupt_sem, &nextActionTime);

		// update next action time
		sem_wait(&statetable_sem);

		if (tables.size() > 0){
			struct timespec now = TimeUtil::getTime();
			// get state from hyper table, updating its next action time
			double f = tables[nextCoreId].getState(now);
			// change the power state of the id
			changePower(nextCoreId,   f);

			// update nextActionTime
			nextActionTime = tables[0].peekNextTime();
			nextCoreId = 0;
			for (int i = 1; i < (int) tables.size(); ++i){
				struct timespec aux = tables[i].peekNextTime();
				if (aux < nextActionTime){
					nextActionTime = aux;
					nextCoreId = i;
				}
			}

		}
		sem_post(&statetable_sem);

	}
}


void PowerManager::setStateTables(const std::vector<StateTable> & newtables){
	if (newtables.size() != tables.size()){
		cerr << "PowerManager::setStateTables: incorrect newtables\n";
		exit(1);
	}

	sem_wait(&statetable_sem);
	// read time after get the semaphore
	struct timespec now = TimeUtil::getTime();
	for (int i = 0; i < (int)tables.size(); ++i){
		tables[i].setStateTable(newtables[i], now);
	}
	// default: next action core is the first one
	nextActionTime = now;
	nextCoreId = 0;
	sem_post(&statetable_sem);

	//unblock power manager 
	sem_post(&interrupt_sem); 
}



void PowerManager::changePower(int id, double f){
	if ( f<EPSILON && f>-EPSILON){
		workers[id]->deactivate();
	}else if (f > 0){
		if (!workers[id]->isActive()){
			workers[id]->activate();
		}	
		setFrequency(id, f);
	}
}

void PowerManager::setFrequency(int id, double f){
	ostringstream freq;
	freq << f;
	system(("echo " + freq.str() + " > " + freqInterface[id]).c_str());
}