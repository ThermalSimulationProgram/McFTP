#include "PowerManager.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "TimeUtil.h"
#include "Operators.h"
using namespace std;

#define EPSILON 0.00001
PowerManager::PowerManager(unsigned _id, std::vector<Worker* > _workers):TimedRunnable(_id), 
workers(_workers){

	freqInterface.push_back("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu2/cpufreq/scaling_setspeed");
	freqInterface.push_back("/sys/devices/system/cpu/cpu3/cpufreq/scaling_setspeed");

}

void PowerManager::join(){
	join_unblock();
	join2();
}

void PowerManager::wrapper(){

	startTimedRun();

	timedRun();



}


void PowerManager::setStateTables(const std::vector<StateTable> & newtables){
	if (newtables.size() != tables.size()){
		cerr << "PowerManager::setStateTables: incorrect newtables\n";
		exit(1);
	}
	struct timespec now = TimeUtil::getTime();

	for (int i = 0; i < (int)tables.size(); ++i){
		tables[i].setStateTable(newtables[i], now);
	}
}


void PowerManager::timedJob(unsigned id){

	// change the power state of the id
	double f = tables[nextCoreId].getState();
	changePower(nextCoreId,   f);

	// update next action time
	if (tables.size() > 0){
		struct timespec nextActionTime = tables[0].peekNextTime();
		nextCoreId = 0;
		for (int i = 1; i < (int) tables.size(); ++i)
		{
			struct timespec aux = tables[i].peekNextTime();
			if (aux < nextActionTime){
				nextActionTime = aux;
				nextCoreId = i;
			}
		}

		setAbsTime(nextActionTime);

	}


	
}


void PowerManager::changePower(int id, double f){
	if ( f<EPSILON && f>-EPSILON){
		workers[id]->deactivate();
	}else if (f > 0){
		workers[id]->activate();
		setFrequency(id, f);
	}
}

void PowerManager::setFrequency(int id, double f){
	ostringstream freq;
	freq << f;
	system(("echo " + freq.str() + " > " + freqInterface[id]).c_str());
}