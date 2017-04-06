#ifndef _SCRATCH_H
#define _SCRATCH_H

#include <vector>
#include <string>
#include <semaphore.h>


#include "utils/Enumerations.h"
#include "structdef.h"




class Scratch{
public:

	static void initialize(int, unsigned long, unsigned long, unsigned long,
		unsigned long, std::vector<unsigned long>, std::vector<unsigned long>,
		enum _schedule_kernel, unsigned long, std::string);


	static void 					setAdaptionPeriod(unsigned long);
	static void 					setName(std::string);
	static void 					setSavingFile(bool);
	static void 					setBenchmark(const std::string&);
	

	static unsigned long 			getAdaptionPeriod();
	static std::string 					getName();
	static int 						getNstage();

	static std::vector<unsigned long> 	getWcets();

	static unsigned long 			getDuration();

	static void 					print();
	static bool 					isSaveFile();
	static std::string 				getBenchmarkName();


private:
	
	static std::string 				name;
	static int 						nstage;
	static unsigned long 			period;
	static unsigned long 			jitter;
	static unsigned long 			distance;
	static unsigned long 			rltDeadline;
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