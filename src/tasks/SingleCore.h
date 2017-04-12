#ifndef SINGLECORE_H
#define SINGLECORE_H 

#include "core/Task.h"


class SingleCore: public Task{
private:

	unsigned long wcet_us;

public:
	SingleCore(unsigned long wcet_us, _task_load_type  load, int loadId, int taskid);

	void fire();
};



#endif
