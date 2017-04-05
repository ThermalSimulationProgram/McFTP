#ifndef POWERMANAGER_H
#define POWERMANAGER_H 

#include <vector>
#include <string>

#include "StateTable.h"
#include "HyperStateTable.h"
#include "TimedRunnable.h"

class Worker{
public:
	void activate(){

	};

	void deactivate(){

	};
};

class PowerManager: public TimedRunnable{
private:
	std::vector<HyperStateTable> tables;
	std::vector<Worker* > workers;

	int nextCoreId;

	std::vector<std::string> freqInterface; 
public:
	PowerManager(unsigned, std::vector<Worker* > workers);

	///gives the PowerManager thread the ACTIVE_PR priority
	void activate();

	///This join function takes into account the PowerManager's unblocking mechanism
	void join();

	///The created thread in the object starts execution by invoking this function 
	void wrapper();

	/// This function is invoked at given time instances to control the power.
	void timedJob(unsigned);

	// void updateNextTime();

	void setStateTables(const std::vector<StateTable> &);

	void changePower(int id, double f);

	void setFrequency(int id, double f);


};




#endif