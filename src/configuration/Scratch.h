#ifndef _SCRATCH_H
#define _SCRATCH_H

#include <vector>
#include <string>
#include <semaphore.h>


#include "utils/Enumerations.h"
#include "utils/TimeUtil.h"
#include "core/structdef.h"




class Scratch{
public:
	
	static void initialize(int, unsigned long, std::string);
	
	
	static void 					setAdaptionPeriod(unsigned long);
	static void 					setName(std::string);
	static void 					setSavingFile(bool);
	static void 					setBenchmark(const std::string&);
	static void 					addTask(_task_type, _task_periodicity, task_data);
	
	
	static unsigned long 			getAdaptionPeriod();
	static std::string 				getName();
	static int 						getNcores();
	
	// static std::vector<unsigned long> 	getWcets();
	
	static unsigned long 			getDuration();
	
	static void 					print();
	static bool 					isSaveFile();
	static std::string 				getBenchmarkName();
	static std::vector<_task_type>	getAllTaskTypes();
	static std::vector<_task_periodicity> 	getAllTaskPeriodicity();
	static std::vector<task_data>  getTaskData();
	static task_data  getTaskData(_task_type);
	static std::string 				getApproachName();
	
	
private:
	
	static std::vector<_task_periodicity> periodicities;
	static std::vector<_task_type> task_types;
	static std::vector<task_data>  all_task_data;
	
	static std::string 				name;
	static int 						nstage;
	
	// static std::vector<double> 			dwcets;
	// static std::vector<unsigned long> 	arrival_times;
	// static enum _schedule_kernel 	kernel;
	static unsigned long 			duration;
	
	// static double 					exefactor;
	// static double 					bfactor;
	
	static unsigned long 			adaption_period;
	// static ptmspec 					ptm;
	// static thermalProp 				offlinedata;
	
	static sem_t 					access_sem;
	static bool 					isSave;
	static std::string 				benchmark;
};

void task_data_print(const task_data &);
	#endif 