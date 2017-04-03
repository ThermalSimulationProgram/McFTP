#ifndef _SATICKERNEL_H
#define _SATICKERNEL_H 



#include <vector>

#include "ScheduleKernel.h"
#include "ScheduleKernelEnumerations.h"


class StaticKernel: public ScheduleKernel{
protected:
	std::vector<double> tons;
	std::vector<double> toffs;

public:
	StaticKernel(unsigned n, enum _schedule_kernel kernel_type);

	~StaticKernel();

	void setStaticScheme(std::vector<double>, std::vector<double>);

	void getScheme(std::vector<double> &, std::vector<double>&);

};








#endif