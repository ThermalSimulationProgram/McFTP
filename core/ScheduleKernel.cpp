#include "ScheduleKernel.h"

// #include "vectormath.h"
// #include "Statistics.h"

#include "ScheduleKernelAPI.h"


ScheduleKernel::ScheduleKernel(unsigned n, 
	enum _schedule_kernel _kernel_type){
	nstages = n;
	kernel_type = _kernel_type;
	scheduleAPI = 0;
}

ScheduleKernel::~ScheduleKernel(){
	
}


void ScheduleKernel::setScheduleAPI(ScheduleKernelAPI* s){
	scheduleAPI = s;
}


// void ScheduleKernel::getScheme(vector<double> & tons, 
// 	vector<double>& toffs){
// 	ScheduleKernel *t = (ScheduleKernel *) this;
// 	t->getScheduleScheme( tons,  toffs);
// }


