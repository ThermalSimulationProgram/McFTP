#include "ScheduleKernelAPI.h"


#include "Scheduler.h"
#include "ScheduleKernel.h"
#include "Scratch.h"
#include "vectormath.h"
#include "Statistics.h"
#include "Pipeline.h"
#include "APTMKernel.h"
#include "BWSKernel.h"
#include "StaticKernel.h"


using namespace std;
ScheduleKernelAPI::ScheduleKernelAPI(Scheduler* scheduler, 
	enum _schedule_kernel kernel_type,  std::vector<unsigned long>& rl_scheduling_times){
	_scheduler = scheduler;
	timeExpense.reserve(10000);
	allschemes.reserve(10000);
	// alltoffs.reserve(10000);

	unsigned nstages = Scratch::getNstage();
	// get the kernel for different kernel types
	switch (kernel_type){
		case APTM : {
		APTMKernel *tempkernel = new APTMKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), Scratch::getBFactor(), kernel_type,
				rl_scheduling_times);

			tempkernel->setOfflineData(Scratch::getOfflineData());
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case BWS: {
			BWSKernel *tempkernel = new BWSKernel(nstages, Scratch::getDwcets(),
				vector<double>(nstages, 2), kernel_type, rl_scheduling_times);
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
		case PBOO:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}
			
		case GE:{
			StaticKernel *tempkernel = new StaticKernel(nstages, kernel_type);
			// tempkernel->setStaticScheme(vector<double>(nstages, 10000), vector<double>(nstages, 0));
			_kernel = (ScheduleKernel*) tempkernel;
			break;
		}

	}

	_kernel->setScheduleAPI(this);
}


void ScheduleKernelAPI::runKernel(vector<double>& tons, 
	vector<double>& toffs){
	double timein = (double)Statistics::getRelativeTime();
	_kernel->getScheme( tons,  toffs);
	double timeout = (double)Statistics::getRelativeTime();
	vector<double> tmp;
	tmp.insert(tmp.end(), tons.begin(), tons.end());
	tmp.insert(tmp.end(), toffs.begin(), toffs.end());
	allschemes.push_back(tmp);
	// alltoffs.push_back(toffs);
	timeExpense.push_back(timeout - timein);
}


double ScheduleKernelAPI::getMaxTimeExpense(){
	return maxElement(timeExpense);
}

double ScheduleKernelAPI::getMeanTimeExpense(){
	double sum = std::accumulate(timeExpense.begin(), timeExpense.end(), 0.0);
	double avg = sum/timeExpense.size();
	return avg;
}

void ScheduleKernelAPI::getPipelineInfo(PipelineInfo& pinfo){
	_scheduler->getPipelinePointer()->getAllPipelineInfo(pinfo);
}

vector<vector<double> > ScheduleKernelAPI::getAllSchemes(){
	return allschemes;
}
