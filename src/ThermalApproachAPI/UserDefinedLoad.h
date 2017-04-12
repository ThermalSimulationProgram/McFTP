#ifndef USERDEFINEDLOAD_H
#define USERDEFINEDLOAD_H


#include "core/TaskLoad.h"

class UserDefinedLoad: public TaskLoad{
private:
	unsigned long count;

	int loadId;

public:
	UserDefinedLoad(int _loadId);
	~UserDefinedLoad();

	void runLoads(unsigned long _wcet_us);

	void coolTask();

};




#endif