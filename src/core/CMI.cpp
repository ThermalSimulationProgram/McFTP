#include "core/CMI.h"


#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

//components
#include "core/TempWatcher.h"
#include "core/Dispatcher.h"
#include "core/ThermalApproach.h"
#include "core/Worker.h"
#include "core/Task.h"

//parser
#include "utils/Parser.h"
//utilities
#include "configuration/Scratch.h"
#include "utils/TimeUtil.h"
#include "utils/Semaphores.h"
#include "pthread/Priorities.h"
#include "results/Statistics.h"
#include "utils/Operators.h"
#include "utils/FileOperator.h"
#include "utils/Enumerations.h"

#include "ThermalApproachAPI/ThermalApproachAPI.h"



using namespace std;
#define _DEBUG 0
#define _INFO 0


bool CMI::initialized = false;
bool CMI::running  = false;

// Constructor needs the xml file path
CMI::CMI(string xml_path):CMI(xml_path, 0){

}

// Constructor needs the xml file path
CMI::CMI(string xml_path, int isAppendSaveFile):cpuUsageRecorder()
{
	Parser* p = new Parser(xml_path);
	if (p->parseFile()!=0){
		cerr << "Pipeline::Pipeline: Cannot parse given xml file!\n";
		exit(1);
	}
	delete p;
	#if _DEBUG == 1
	cout << "finished parse file\n";
	#endif

	// compare stage number with CPU number
	int _n_used = Scratch::getNstage();
	n_cpus        = (int)sysconf(_SC_NPROCESSORS_ONLN);
	if (_n_used <= 0 || _n_used > n_cpus){
		cerr << "Invalid stage number, should be a positive integer which is less than n_cpus";
		pthread_exit(0);
	}
	
	n_used        = _n_used;


	// create Dispatchers
	vector<_task_type> allTaskTypes = Scratch::getAllTaskTypes();
	vector<_task_type> allTaskPeriodicity = Scratch::getAllTaskPeriodicity();

	for (int i = 0; i < (int) allTaskTypes.size(); ++i)
	{
		// for (int j = 0; i < (int) allTaskTypes.size(); ++j)
		// {
		// 	if (j == i){
		// 		continue;
		// 	}
		// 	if(allTaskTypes[j] == allTaskTypes[i]){
		// 		cerr << "CMI::CMI: duplicated task type" << endl;
		// 		exit(1);
		// 	}
		// }

		switch (allTaskPeriodicity[i]){
			case aperiodic:{
				Aperiodic* aux = Aperiodic(100+i, allTaskTypes[i]);
				break;
			}
			case periodic:{
				Periodic* aux = Periodic(100+i, allTaskTypes[i]);
				break;
			}
			case periodic_jitter:{
				PeriodicJitter* aux = PeriodicJitter(100+i, allTaskTypes[i]);
				break;
			}

		}

		dispatchers.push_back((Dispatcher*)aux);
		
	}
	

	thermal_approach       = new ThermalApproach(99, this, Scratch::getApproachName());
	
	// create n_used workers and attach it to default CPU
	for (int i = 0; i < n_used; ++i){
		Worker *t = new Worker(i, i);
		workers.push_back(t);
		worker_cpu.push_back(i);		
	}

	// give main thread the highest priority
	param.sched_priority = Priorities::get_main_pr(); //highest priority
	if ((sched_setscheduler(0, SCHED_FIFO, &param) != 0)) {
		cout << "Run with root\n";
		pthread_exit(0);
	}
	tempwatcher 	= new TempWatcher(200000, 98);

	_isAppendSaveFile = isAppendSaveFile;
	// initialize();

	for (int i = 0; i < n_used; ++i)
	{
		sem_t aux;
		sem_init(&aux, 0, 1);
		taskqueue_sems.push_back(aux);
		sem_t aux2;
		sem_init(&aux2, 0, 0);
		jobnumber_sems.push_back(aux2);

		JobQueue temp_q();
		allTaskQueues.push_back(temp_q);
	}

}

CMI::~CMI(){
	delete thermal_approach;
	delete tempwatcher;

	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		delete dispatchers[i];
	}
	for (int i = 0; i < (int) workers.size(); ++i){
		delete workers[i];
	}
	
}

// prepare for simulation, initialize statistics, dispatcher, scheduler
void CMI::initialize(){
	Statistics::initialize();


	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->trigger();
	}
	

	thermal_approach->trigger();
	thermal_approach->activate();

	tempwatcher->trigger();

	Worker *current;
	for (int i = 0; i < (int) workers.size(); ++i){
		current->setCMI(this);
	}

	// Main thread waits for all threads initialized, especially the scheduler
	Semaphores::rtcinit_sem.wait_sem();
	// all threads initialized
	initialized = true;
}



// explicitly set the CPUs to which the works are attached 
void CMI::setWorkerCPU(vector<unsigned> order){
	if(order.size()!= (unsigned)n_stages){
		cout<<"CMI::setWorkerCPU: invalid input\n";
		return;
	}
	for (unsigned i = 0; i < order.size(); ++i){
		if (order[i]<0 || order[i]>= (unsigned)n_cpus){
			cout<<"CMI::setWorkerCPU: invalid input\n";
			return;
		}
	}
	worker_cpu = order;
}


// Start the simulation, the duration is loaded from Scratch class
double CMI::simulate(){
	Worker *t;
	// activate all the threads and attach them to their CPUs.
	for (unsigned i = 0; i < workers.size(); ++i)
	{
		t  = workers[i];
		t->trigger();
		t->activate();
		t->setCPU(worker_cpu[i]);
	}
	
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->activate();
		dispatchers[i]->setCPU(n_cpus-1);
	}
	
	
	thermal_approach->setCPU(n_cpus - 2);

	tempwatcher->activate();
	if(workers.size()==1){
		tempwatcher->setCPU(n_cpus - worker_cpu[0] - 1);
	}
	else
		tempwatcher->setCPU(n_cpus - 1);
		
	
	Statistics::enable();
	Statistics::start();

	unsigned long tstart_us = TimeUtil::convert_us(Statistics::getStart());

	// simulation starts now
	cpuUsageRecorder.startLoggingCPU();
	// record start time and send it to TimedRunnable objects 
	struct timespec rem;
	struct timespec duration = TimeUtil::Micros(Scratch::getDuration());

	running = true;
	// main thread sleeps for duration time length
	nanosleep(&duration, &rem);
	running = false;
	// join other threads, wait them to finish
	join_all();
	// return the average temperature 

	cpuUsageRecorder.endLoggingCPU();

	saveResults();
	
	return tempwatcher->getMeanMaxTemp();
}


void CMI::join_all() {
	Worker* t;
   	#if _DEBUG==1
  		cout << "Joining all threads...\n";
    #endif

	for( unsigned i=0;i<workers.size();i++) {
		t = workers[i];
		if(t!=NULL) {
			t->join();
		}
	}

	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		if (dispatchers[i] != NULL){
			dispatchers[i]->join();
		}
	}

	thermal_approach->join();
	tempwatcher->join();
    #if _INFO==1
  		cout << "Joined all!\n";
    #endif
}


void CMI::updateConfiguration(const Configuration& c){
	powermanager->setStateTables(c.getAllStateTables);

	// for (int i = 0; i < (int) workers.size(); ++i)
	// {
	// 	workers[i].setJobQueue(c.getJobQueue(i));
	// }

	tryLockTaskQueues();
	allTaskQueue = c.getAllJobQueue();
	unlockTaskQueues();

}


// This function is called by the dispatcher to release a new job
void CMI::newJob(_task_type TASK_TYPE){
	// #if _INFO == 1
	// Semaphores::print_sem.wait_sem();
	// cout << "New job with id: " << j->getId() << " released at time: " 
	//   << (Statistics::getRelativeTime_ms()) << " microsecond! The accurate time is " 
	//   << accurate_time << endl;
	// Semaphores::print_sem.post_sem();
	// #endif
	
	int coreId = ThermalApproachAPI::addNewJob(TASK_TYPE, this);

	switch (TASK_TYPE){
		case busywait:
		case benchmark:
		case userdefined:
		break;
	}
	sem_wait(&taskqueue_sems[coreId]);
	allTaskQueue[coreId].insertJob(newTask);
	sem_post(&jobnumber_sems[coreId]);
	sem_post(&taskqueue_sems[coreId]);

	// workers[coreId].addJob(newTask);
}

Task* CMI::tryLoadJob(int workerId){
	Task* ret;
	sem_wait(&taskqueue_sems[workerId]);

	sem_wait(&jobnumber_sems[workerId]);
	ret = allTaskQueue[workerId].pop_front();

	sem_post(&taskqueue_sems[workerId]);
	return ret;
}

void CMI::lockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_wait(&taskqueue_sems[i]);
	}

}

void CMI::tryLockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_trywait(&taskqueue_sems[i]);
	}

}

void CMI::unlockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_post(&taskqueue_sems[i]);
	}

}


// This function is called by the end stage to announce a job is finished
void CMI::finishedJob(Task* t){
	Configuration ThermalApproachAPI::finishJob(t);
}


// Interface function to get member 'initialized'
bool CMI::isInitialized(){
	return initialized;
}
// Interface function to get member 'simulating'
bool CMI::isSimulating(){
	return running;
}


// This function is called by the scheduler to apply new schedule scheme to each stage
void CMI::setPTMs(vector<unsigned long> ton, vector<unsigned long> toff){
	// simply forwards the ton and toff to each stage
	for (unsigned i = 0; i < workers.size(); ++i)
		workers[i]->setPTM(ton[i], toff[i]);
}


unsigned CMI::getNCPU(){
	return n_cpus;
}


void CMI::getDynamicInfo(DynamicInfo& pinfo){

	pinfo.workerinfos.clear();
	pinfo.temperature = tempwatcher->getCurTemp();
	pinfo.numstages   = n_used;

	// get current time from simulation start, unit ms
	double curTime = (double)Statistics::getRelativeTime()/1000;

	pinfo.currentTime = curTime;

	for (int i = 0; i < n_stages; ++i){
		WorkerInfo tmp;
		workers[i]->getAllInfo(curTime, tmp);
		pinfo.workerinfos.push_back(tmp);
	}	
}

void CMI::saveResults(){
	if (Scratch::isSaveFile()){

		// string tempSaveName = Scratch::getName() + "_result.csv";

		// vector<string> beginOfData = vector<string>(1, "111111111111111111111111111111");
		

		// if (_isAppendSaveFile > 0){
		// 	appendContentToFile(tempSaveName, beginOfData);
		// }else{
		// 	saveContentToNewFile(tempSaveName, beginOfData);
		// }
		
		// appendToFile(tempSaveName, tempwatcher->getMeanTemp());
		// double maxTemp = tempwatcher->getMaxTemp();
		// appendToFile(tempSaveName, vector<double>(1, maxTemp));

		// double MeanMaxTemp = tempwatcher->getMeanMaxTemp();
		// appendToFile(tempSaveName, vector<double>(1, MeanMaxTemp));

		// appendToFile(tempSaveName, scheduler->getKernelTime());

		// float total_cpu_usage = cpuUsageRecorder.getUsage();
		// appendToFile(tempSaveName, vector<float>(1, total_cpu_usage));

		// appendContentToFile(tempSaveName, Statistics::getAllMissedDeadline());

		// vector<string> endOfData = vector<string>(1, "999999999999999999999999999999");
		// appendContentToFile(tempSaveName, endOfData);
		// appendToFile(tempSaveName, scheduler->getAllSchemes());
	}
}