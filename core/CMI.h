#ifndef _CMI_H
#define _CMI_H

#include <vector>
#include <string>

#include "core/structdef.h"
#include "results/CPUUsage.h"
#include "utils/Enumerations.h"

class Worker;
class Dispatcher;
class Scheduler;
class TempWatcher;
class Job;
class CPUUsage;
class PowerManager;


class CMI{
protected:
	// This attribute indicates if the McFTP is initialized
	static bool initialized;

	// This attribute indicates if the McFTP is running
	static bool running;

	// number of used cores, should be set in the xml file
	int n_used;

	// number of cpu cores in the hardware platform
	int n_cpus;

	// This vector stores the cpu core ids corresponding to each core
	std::vector<unsigned> worker_cpu;

	// This vector stores pointers to workers. Each worker represents 
	// a core.
	std::vector<Worker*> workers;

	// Dispatcher release jobs to CMI
	Dispatcher 	*dispatcher;

	// Scheduler manages the execution of the workers
	Scheduler 	*scheduler;

	// TempWatcher periodically reads the temperatures of the cpu cores and 
	// records them
	TempWatcher *tempwatcher;

	// PowerManager controls the power dissipation of the cores according to
	// StateTables given by thermal approaches
	PowerManager * powermanager;

	// auxiliary variable to set main thread priority
	struct sched_param param;

	// CPUUsage records cpu active and idle times, used to calculate cpu usage
	CPUUsage cpuUsageRecorder;

	int _isAppendSaveFile;
public:

	// Constructor needs the xml file path
	explicit CMI(std::string);

	// Constructor needs the xml file path
	explicit CMI(std::string, int);

	~CMI();

	/************** Simulation interface functions ************/
	// prepare for simulation, initialize statistics, dispatcher, scheduler
	void initialize();

	// explicitly set the CPUs to which the workers are attached 
	void setWorkerCPU(std::vector<unsigned>);

	// Start the simulation, the duration is loaded from Scratch class
	double simulate();

	// join other threads, wait them to finish
	void join_all();

	unsigned getNCPU();



	/************** Simulation functions ************/
	// This function is called by the dispatcher to release a new job
	void newJob(_task_type task_type);

	// call this function to stop the simulation
	void endSimulation();

	// Interface function to get member 'initialized'
	static bool isInitialized();

	// Interface function to get member 'running'
	static bool isRunning();

	// This function is called by the work 
	void finishedJob(Task*);

	// This function is called by the scheduler to apply new schedule scheme to 
	// each stage
	void setPTMs(std::vector<unsigned long>, std::vector<unsigned long>);

	// This function collects all the information of the pipeline
	// in the simulation
	void getDynamicInfo(PipelineInfo& p);


};




#endif