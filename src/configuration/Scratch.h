#ifndef _SCRATCH_H
#define _SCRATCH_H

#include <vector>
#include <string>
#include <time.h>
#include <semaphore.h>


#include "utils/Enumerations.h"
#include "utils/TimeUtil.h"
#include "core/structdef.h"
#include "core/TaskArgument.h"



class Scratch{
public:
	
	static void initialize(int, unsigned long, std::string);
	
	
	static void 					setAdaptionPeriod(unsigned long);
	static void 					setName(std::string);
	static void 					setSavingFile(bool);
	static void 					setBenchmark(const std::string&);
	static void 					addTask(_task_type, _task_periodicity, TaskArgument);
	static void 					setWCETs(std::vector<struct timespec>);
	static void 					setFixedFrequency(bool);
	static void 					setFixedActive(bool);
	static void 					setStaticApproach(bool);
	static void 					setPBOOTons(std::vector<struct timespec>);
	static void 					setPBOOToffs(std::vector<struct timespec>);



	static bool 					isFixedFrequency();
	static bool 					isFixedActive();
	static bool 					isStaticApproach();
	static std::vector<struct timespec> getPBOOTons();
	static std::vector<struct timespec> getPBOOToffs();
	
	static unsigned long 			getAdaptionPeriod();
	static std::string 				getName();
	static int 						getNcores();
	
	static std::vector<struct timespec> 	getWCETs();
	
	static unsigned long 			getDuration();
	
	static void 					print();
	static bool 					isSaveFile();
	static std::string 				getBenchmarkName();
	static std::vector<_task_type>	getAllTaskTypes();
	static std::vector<_task_periodicity> 	getAllTaskPeriodicity();
	static std::vector<TaskArgument>  getTaskData();
	static TaskArgument  getTaskData(int taskid);
	static std::string 				getApproachName();
	
	
private:

	static bool 					fixedFrequency;
	static bool 					fixedActive;
	static bool 					is_staticApproach;
	static std::vector<struct timespec> 	PBOO_tons;
	static std::vector<struct timespec> 	PBOO_toffs;
	
	static std::vector<_task_periodicity> periodicities;
	static std::vector<_task_type> task_types;
	static std::vector<TaskArgument>  all_task_data;
	
	static std::string 				name;
	static int 						nstage;
	static std::vector<struct timespec> 	WCETS;
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

// void task_data_print(const task_data &);
	#endif 