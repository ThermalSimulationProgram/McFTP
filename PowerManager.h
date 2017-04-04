#ifndef POWERMANAGER_H
#define POWERMANAGER_H 

#include <vector>


#include "StateTable.h"
#include "TimedRunnable.h"

class Worker;

class PowerManager: public TimedRunnable{
private:
	std::vector<StateTable> tables;
	std::vector<Worker* > workers;

	std::vector<int> nextCoreId;

public:
	PowerManager(unsigned);

	///gives the PowerManager thread the ACTIVE_PR priority
	void activate();

	///This join function takes into account the PowerManager's unblocking mechanism
	void join();

	///The created thread in the object starts execution by invoking this function 
	void wrapper();

	/// This function is invoked at given time instances to control the power.
	void timedJob(unsigned);

	void updateNextTime();

	void setStateTables();

};




#endif