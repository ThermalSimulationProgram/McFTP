#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <vector>
#include <string>
#include <time.h>

#include <semaphore.h>

#include "pthread/Thread.h"
#include "core/CMI.h"




class Processor;

class ThermalApproach: public Thread
{
protected:

	/////////////////////////////////////////////
	struct timespec period;
	
	Processor* processor;

	std::string approachName;

	std::vector<double> timeExpense;

	online_thermal_approach onlineApproach;

	offline_thermal_approach offlineApproach;

public:
	ThermalApproach(unsigned _id, Processor *c, std::string name);
	
	~ThermalApproach();

	void init();

	void activate();

	void wrapper();

	void join();

	std::vector<double> getKernelTime();

	void setPeriod(unsigned long period_us);

	void setOnlineApproach(online_thermal_approach newapproach);

	void setOfflineApproach(offline_thermal_approach newapproach);

};






#endif