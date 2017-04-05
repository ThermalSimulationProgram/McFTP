#ifndef _SCHEDULEKERNEL_H
#define _SCHEDULEKERNEL_H 

#include <vector>


#include "ScheduleKernelEnumerations.h"


class ScheduleKernelAPI;

class ScheduleKernel{
protected:
	//static data
	unsigned nstages;
	
	enum _schedule_kernel kernel_type;

	ScheduleKernelAPI* scheduleAPI;

public:
	ScheduleKernel(unsigned n, enum _schedule_kernel kernel_type);
	virtual ~ScheduleKernel();
	
	void setScheduleAPI(ScheduleKernelAPI*);

	// void getScheme(vector<double> &, vector<double>&);
	virtual void getScheme(std::vector<double> &, std::vector<double>&) = 0;


	
};


#endif