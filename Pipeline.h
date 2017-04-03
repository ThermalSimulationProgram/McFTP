#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <vector>
#include <string>

#include "structdef.h"
#include "CPUUsage.h"

class Worker;
class Dispatcher;
class Scheduler;
class TempWatcher;
class Job;
class CPUUsage;


class Pipeline{
protected:
	// This attribute indicates if the Pipeline is initialized
	static bool initialized;

	// This attribute indicates if the Pipeline is simulating
	static bool simulating;

	// number of pipeline stages
	int n_stages;

	// number of cpu cores in the  processor
	int n_cpus;

	// This vector stores the cpu core ids corresponding to each stage
	std::vector<unsigned> worker_cpu;

	// This vector stores pointers to workers. Each worker represents 
	// a stage.
	std::vector<Worker*> workers;

	// Dispatcher release jobs to Pipeline
	Dispatcher 	*dispatcher;

	// Scheduler manages the execution of the workers
	Scheduler 	*scheduler;

	// TempWatcher periodically reads the temperatures of the cpu cores and 
	// records them
	TempWatcher *tempwatcher;

	// auxiliary variable to set main thread priority
	struct sched_param param;

	// CPUUsage records cpu active and idle times, used to calculate cpu usage
	CPUUsage cpuUsageRecorder;

	int _isAppendSaveFile;
public:

	// Constructor needs the xml file path
	explicit Pipeline(std::string);

	// Constructor needs the xml file path
	explicit Pipeline(std::string, int);

	~Pipeline();

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
	void newJob(Job*, long);

	// call this function to stop the simulation
	void endSimulation();

	// Interface function to get member 'initialized'
	static bool isInitialized();

	// Interface function to get member 'simulating'
	static bool isSimulating();

	// This function is called by the end stage to announce a job is finished
	void finishedJob(Job*);

	// This function is called by the scheduler to apply new schedule scheme to 
	// each stage
	void setPTMs(std::vector<unsigned long>, std::vector<unsigned long>);

	// This function collects all the information of the pipeline
	// in the simulation
	void getAllPipelineInfo(PipelineInfo& p);






	/************** Functions for APTM and BWS kernels ************/
	// This function collects the dynamic information of the pipeline
	// in the simulation, and save it in config		
	// void getInfo(pipeinfo&, const vector<double>&, const vector<double>&, 
	// 	enum _schedule_kernel kernel);



	// This function is used for debugging, not used in real program
	// static void loadInfoFromFile(pipeinfo&, const vector<double>&, 
	// 	const vector<double>&, enum _schedule_kernel kernel);


};




#endif