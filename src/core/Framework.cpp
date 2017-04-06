#include "core/Framework.h"



#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

//components
#include "core/Framework.h"
#include "core/PowerManager.h"
#include "core/TempWatcher.h"
#include "core/Dispatcher.h"
#include "core/ThermalApproach.h"
#include "core/Worker.h"
#include "core/Task.h"
#include "dispatchers/Aperiodic.h"
#include "dispatchers/Periodic.h"
#include "dispatchers/PeriodicJitter.h"
#include "tasks/BusyWait.h"

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


bool Framework::initialized = false;
bool Framework::running  = false;

// Constructor needs the xml file path
Framework::Framework(string xml_path):Framework(xml_path, 0){

}

// Constructor needs the xml file path
Framework::Framework(string xml_path, int isAppendSaveFile):cpuUsageRecorder()
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
	vector<_task_periodicity> allTaskPeriodicity = Scratch::getAllTaskPeriodicity();

	for (int i = 0; i < (int) allTaskTypes.size(); ++i)
	{
		// for (int j = 0; i < (int) allTaskTypes.size(); ++j)
		// {
		// 	if (j == i){
		// 		continue;
		// 	}
		// 	if(allTaskTypes[j] == allTaskTypes[i]){
		// 		cerr << "Framework::Framework: duplicated task type" << endl;
		// 		exit(1);
		// 	}
		// }

		switch (allTaskPeriodicity[i]){
			case aperiodic:{
				Aperiodic* aux = new Aperiodic(100+i, allTaskTypes[i]);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}
			case periodic:{
				Periodic* aux = new Periodic(100+i, allTaskTypes[i]);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}
			case periodic_jitter:{
				PeriodicJitter* aux = new PeriodicJitter(100+i, allTaskTypes[i]);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}

		}

		
		
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

		JobQueue temp_q = JobQueue();
		allTaskQueues.push_back(temp_q);
	}

}

Framework::~Framework(){
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
void Framework::initialize(){
	Statistics::initialize();


	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->trigger();
	}
	

	thermal_approach->trigger();
	thermal_approach->activate();

	tempwatcher->trigger();

	
	for (int i = 0; i < (int) workers.size(); ++i){
		workers[i]->setFramework(this);
	}

	// Main thread waits for all threads initialized, especially the scheduler
	Semaphores::rtcinit_sem.wait_sem();
	// all threads initialized
	initialized = true;
}



// explicitly set the CPUs to which the works are attached 
void Framework::setWorkerCPU(vector<unsigned> order){
	if(order.size()!= (unsigned)n_used){
		cout<<"Framework::setWorkerCPU: invalid input\n";
		return;
	}
	for (unsigned i = 0; i < order.size(); ++i){
		if (order[i]<0 || order[i]>= (unsigned)n_cpus){
			cout<<"Framework::setWorkerCPU: invalid input\n";
			return;
		}
	}
	worker_cpu = order;
}


// Start the simulation, the duration is loaded from Scratch class
double Framework::simulate(){
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


void Framework::join_all() {
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



Task* Framework::tryLoadJob(int workerId){
	Task* ret;
	sem_wait(&taskqueue_sems[workerId]);

	sem_wait(&jobnumber_sems[workerId]);
	ret = allTaskQueues[workerId].pop_front();

	sem_post(&taskqueue_sems[workerId]);
	return ret;
}

void Framework::lockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_wait(&taskqueue_sems[i]);
	}

}

void Framework::tryLockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_trywait(&taskqueue_sems[i]);
	}

}

void Framework::unlockTaskQueues(){
	for (int i = 0; i < n_used; ++i)
	{
		sem_post(&taskqueue_sems[i]);
	}

}



void Framework::saveResults(){
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