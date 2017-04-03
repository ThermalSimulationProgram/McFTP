#include "StaticKernel.h"

#include "Scratch.h"
#include "structdef.h"

using namespace std;
StaticKernel::StaticKernel(unsigned n, enum _schedule_kernel kernel_type)
:ScheduleKernel(n, kernel_type){
	ptmspec p = Scratch::getPTMValues();
	tons = p.tons;
	toffs = p.toffs;
}

StaticKernel::~StaticKernel(){
	
}

void StaticKernel::setStaticScheme(vector<double> _tons, 
	vector<double> _toffs){
	tons = _tons;
	toffs = _toffs;
}


void StaticKernel::getScheme(vector<double>& tonsin, 
	vector<double>& toffsin){
	tonsin = tons;
	toffsin = toffs;
}