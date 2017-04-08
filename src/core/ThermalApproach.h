#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <vector>
#include <string>
#include <time.h>

#include <semaphore.h>

#include "pthread/Thread.h"




class CMI;

class ThermalApproach: public Thread
{
protected:

	/////////////////////////////////////////////
	struct timespec period;
	
	CMI* cmi;

	std::string approachName;

	std::vector<double> timeExpense;

	bool isStatic;

public:
	ThermalApproach(unsigned _id, CMI *c, std::string name);
	
	~ThermalApproach();

	void init();

	void activate();

	void wrapper();

	void join();

	std::vector<double> getKernelTime();

	

};






#endif