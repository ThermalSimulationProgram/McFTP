#ifndef BUSYWAIT_H
#define BUSYWAIT_H 



#include "core/TaskLoad.h"

class BusyWait : public TaskLoad{
private:

	unsigned long count;

public:
	BusyWait();

	virtual ~BusyWait();

	// void setWCET();

	void runLoads(unsigned long wcet_us);

};












#endif