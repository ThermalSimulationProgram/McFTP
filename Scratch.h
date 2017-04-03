#ifndef _SCRATCH_H
#define _SCRATCH_H

#include <vector>
#include <string>
#include <semaphore.h>


#include "Enumerations.h"
#include "ScheduleKernelEnumerations.h"
#include "structdef.h"
#include "rtc.h"




class Scratch{
public:

	static void initialize(int, unsigned long, unsigned long, unsigned long,
		unsigned long, std::vector<unsigned long>, std::vector<unsigned long>,
		enum _schedule_kernel, unsigned long, std::string);


	static void 					setAdaptionPeriod(unsigned long);
	static void 					setPTMValues(ptmspec);
	static void 					setOfflineData(thermalProp);
	static void 					setExeFactor(double);
	static void 					setBFactor(double f);
	static void 					setName(std::string);
	static void 					setSavingFile(bool);
	static void 					setBenchmark(const std::string&);
	

	static unsigned long 			getAdaptionPeriod();
	static thermalProp 				getOfflineData();
	static ptmspec 					getPTMValues();
	static std::string 					getName();
	static int 						getNstage();
	static unsigned long 			getPeriod();
	static unsigned long 			getJitter();
	static unsigned long 			getDistance();
	static unsigned long 			getRltDeadline();
	static double 					getRltDeadline_ms();
	static std::vector<unsigned long> 	getWcets();
	static std::vector<double> 			getDwcets();
	static std::vector<unsigned long> 	getArrivalTimes();
	static std::vector<double> 			getArrivalTimes_ms();
	static enum _schedule_kernel 	getKernel();
	static unsigned long 			getDuration();
	static double 					getExeFactor();
	static double 					getBFactor();
	static void 					print();
	static bool 					isSaveFile();
	static std::string 				getBenchmarkName();


private:
	
	static std::string 					name;
	static int 						nstage;
	static unsigned long 			period;
	static unsigned long 			jitter;
	static unsigned long 			distance;
	static unsigned long 			rltDeadline;
	static double 					rltDeadline_ms;
	static std::vector<unsigned long> 	wcets;
	static std::vector<double> 			dwcets;
	static std::vector<unsigned long> 	arrival_times;
	static enum _schedule_kernel 	kernel;
	static unsigned long 			duration;

	static double 					exefactor;
	static double 					bfactor;

	static unsigned long 			adaption_period;
	static ptmspec 					ptm;
	static thermalProp 				offlinedata;

	static sem_t 					access_sem;
	static bool 					isSave;
	static std::string 				benchmark;
};


#endif 