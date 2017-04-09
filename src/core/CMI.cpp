#include "core/CMI.h"


#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

//components
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
#include "utils/vectormath.h"


#include "ThermalApproachAPI/ThermalApproachAPI.h"



using namespace std;
#define _DEBUG 0
#define _INFO 0


bool CMI::initialized = false;
bool CMI::running  = false;
sem_t CMI::init_sem;
sem_t CMI::running_sem;

// Constructor needs the xml file path
CMI::CMI(string xml_path):CMI(xml_path, 0){

}

// Constructor needs the xml file path
CMI::CMI(string xml_path, int isAppendSaveFile):cpuUsageRecorder()
{
	// give main thread the highest priority
	param.sched_priority = Priorities::get_main_pr(); //highest priority
	if ((sched_setscheduler(0, SCHED_FIFO, &param) != 0)) {
		cout << "Run with root\n";
		pthread_exit(0);
	}

	sem_init(&init_sem, 0, 0);
	sem_init(&running_sem, 0, 0);
	thread_num = 0;

	Parser* p = new Parser(xml_path);

	if (p->parseFile()!=0){
		cerr << "Pipeline::Pipeline: Cannot parse given xml file!\n";
		exit(1);
	}
	// exit(1);
	
	delete p;
	#if _DEBUG == 1
	cout << "finished parse file\n";
	#endif

	// compare stage number with CPU number
	int _n_used = Scratch::getNcores();
	n_cpus        = (int)sysconf(_SC_NPROCESSORS_ONLN);
	if (_n_used <= 0 || _n_used > n_cpus){
		cerr << "Invalid stage number, should be a positive integer which is less than n_cpus";
		pthread_exit(0);
	}
	n_used        = _n_used;


	// create Dispatchers
	vector<_task_type> allTaskTypes = Scratch::getAllTaskTypes();
	vector<_task_periodicity> allTaskPeriodicity = Scratch::getAllTaskPeriodicity();
	vector<task_data> allTaskData =   Scratch::getTaskData();
	for (int i = 0; i < (int) allTaskTypes.size(); ++i)
	{
		switch (allTaskPeriodicity[i]){
			case aperiodic:{
				Aperiodic* aux = new Aperiodic(100+i, allTaskTypes[i]);
				aux->setReleaseTime(allTaskData[i].release_time);
				aux->setCMI(this);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}
			case periodic:{
				Periodic* aux = new Periodic(100+i, allTaskTypes[i]);
				aux->setPeriod(allTaskData[i].period);
				aux->setCMI(this);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}
			case periodic_jitter:{
				PeriodicJitter* aux = new PeriodicJitter(100+i, allTaskTypes[i]);
				aux->setPeriod(allTaskData[i].period);
				aux->setPeriod(allTaskData[i].jitter);
				aux->setCMI(this);
				dispatchers.push_back((Dispatcher*)aux);
				break;
			}

		}
		++thread_num;
		dispatchers[i]->setTaskData(allTaskData[i]);

		
		
	}
	

	thermal_approach       = new ThermalApproach(99, this, Scratch::getApproachName());
	++thread_num;
	
	// create n_used workers and attach it to default CPU
	for (int i = 0; i < n_used; ++i){
		Worker *t = new Worker(i, i);
		t->setCMI(this);
		workers.push_back(t);
		worker_cpu.push_back(i);
		++thread_num;		
	}

	
	tempwatcher 	= new TempWatcher(200000, 98);
	++thread_num;

	powermanager 	= new PowerManager(200, workers);
	++thread_num;

	_isAppendSaveFile = isAppendSaveFile;

	for (int i = 0; i < n_used; ++i)
	{
		sem_t aux;
		sem_init(&aux, 0, 2); // at most two thread can access, a worker and a dispatcher
		taskqueue_sems.push_back(aux);

		// int v;
		// sem_getvalue(&taskqueue_sems[i], &v);
		// cout << "semaphore value: " << v << endl;

		sem_t aux2;
		sem_init(&aux2, 0, 0);
		jobnumber_sems.push_back(aux2);

		JobQueue temp_q = JobQueue();
		allTaskQueues.push_back(temp_q);
	}

}

CMI::~CMI(){
	delete thermal_approach;
	delete tempwatcher;
	delete powermanager;

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

	for (int i = 0; i < (int) workers.size(); ++i){
		workers[i]->setCMI(this);
	}

	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->trigger();
	}
	
	powermanager->trigger();

	thermal_approach->trigger();
	thermal_approach->setCPU(n_cpus - 1);
	thermal_approach->activate();

	

	
	tempwatcher->trigger();

	// Main thread waits for all threads initialized, especially the scheduler
	Semaphores::rtcinit_sem.wait_sem();
	// all threads initialized
	initialized = true;
	for (int i = 0; i < thread_num; ++i){
		sem_post(&init_sem);
	}
	

}



// explicitly set the CPUs to which the works are attached 
void CMI::setWorkerCPU(vector<unsigned> order){
	if(order.size()!= (unsigned)n_used){
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
		t->setCPU(worker_cpu[i]);
		t->activate();	
	}
	
	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		dispatchers[i]->setCPU(n_cpus-1);
		dispatchers[i]->activate();
		
	}

	
	

	tempwatcher->activate();
	if(workers.size()==1){
		tempwatcher->setCPU(n_cpus - worker_cpu[0] - 1);
	}
	else
		tempwatcher->setCPU(n_cpus - 1);
		
	powermanager->setCPU(6);
	powermanager->activate();

	Statistics::enable();
	
	cout << "start simulation" << endl;

	// simulation starts now
	cpuUsageRecorder.startLoggingCPU();
	// record start time and send it to TimedRunnable objects 
	struct timespec rem;
	struct timespec duration = TimeUtil::Micros(Scratch::getDuration());

	running = true;
	
	for (int i = 0; i < thread_num; ++i){
		sem_post(&running_sem);
	}


	Statistics::start();
	// main thread sleeps for duration time length
	nanosleep(&duration, &rem);
	running = false;
	for (int i = 0; i < n_used; ++i)
	{
		sem_post(&jobnumber_sems[i]);
		sem_post(&jobnumber_sems[i]);
		sem_post(&taskqueue_sems[i]);
		sem_post(&taskqueue_sems[i]);
	}

	// join other threads, wait them to finish
	join_all();
	enum _thread_type main_type = _main;
	Statistics::addRuntime(main_type, 1000, TimeUtil::getTime() - Statistics::getStart());

	std::vector<task_data> allTaskData = Scratch::getTaskData();
	int task_num = allTaskData.size();
	for (int i = 0; i < (int) workers.size(); ++i)
	{
		struct timespec this_worker_job_time = TimeUtil::Micros(0);
		int this_worker_id = workers[i]->getWorkerId();
		
		for (int j = 0; j < task_num; ++j)
		{
			int counter = Statistics::getJobLogNumber(this_worker_id, 
				allTaskData[j].taskId);
			this_worker_job_time = this_worker_job_time + counter*allTaskData[j].wcets[this_worker_id];
		}

		Statistics::addRuntime(_worker, workers[i]->getId(), this_worker_job_time);
	}

	// return the average temperature 
	// cout << "cpuUsageRecorder..." << endl;
	cpuUsageRecorder.endLoggingCPU();

	saveResults();
	
	return tempwatcher->getMeanMaxTemp();
}


void CMI::join_all() {

	for (int i = 0; i < (int) dispatchers.size(); ++i)
	{
		if (dispatchers[i] != NULL){
			dispatchers[i]->join();
		}
	}


	thermal_approach->join();
	
	tempwatcher->join();
	
	powermanager->join();

	Worker* t;
	for( unsigned i=0;i<workers.size();i++) {
		t = workers[i];
		if(t!=NULL) {
			t->join();
		}
	}


    #if _INFO==1
  		cout << "Joined all!\n";
    #endif
}


void CMI::updateConfiguration(const Configuration& config){
	powermanager->setStateTables(config.getAllStateTables());

	// tryLockTaskQueues();
	// allTaskQueues = config.getAllJobQueue();
	// unlockTaskQueues();

}


// This function is called by the dispatcher to release a new job
void CMI::newJob(Task * newTask, _task_type TASK_TYPE){
	
	
	int coreId = ThermalApproachAPI::addNewJob(TASK_TYPE, this);

	insertJobToQueue(coreId, newTask);

	// #if _INFO == 1
	// Semaphores::print_sem.wait_sem();
	// cout << "New job with type: " << TASK_TYPE << " released at time: " 
	//   << (Statistics::getRelativeTime_ms()) << " microsecond!"  << endl;
	// Semaphores::print_sem.post_sem();
	// #endif
}

void CMI::insertJobToQueue(int queueId, Task* newTask){
	sem_wait(&taskqueue_sems[queueId]);
	allTaskQueues[queueId].insertJob(newTask);
	sem_post(&jobnumber_sems[queueId]);
	sem_post(&taskqueue_sems[queueId]);
}


Task* CMI::tryLoadJob(int workerId){
	Task* ret = NULL;

	// 

	sem_wait(&taskqueue_sems[workerId]);
	sem_wait(&jobnumber_sems[workerId]);
	ret = allTaskQueues[workerId].pop_front();

	sem_post(&taskqueue_sems[workerId]);
	return ret;
}




void CMI::getDynamicInfo(DynamicInfo& pinfo){

	pinfo.workerinfos.clear();
	pinfo.temperature = tempwatcher->getCurTemp();
	pinfo.numstages   = n_used;

	// get current time from simulation start, unit ms
	double curTime = (double)Statistics::getRelativeTime()/1000;

	pinfo.currentTime = curTime;

	// for (int i = 0; i < n_used; ++i){
	// 	WorkerInfo tmp;
	// 	workers[i]->getAllInfo(curTime, tmp);
	// 	pinfo.workerinfos.push_back(tmp);
	// }	
}


struct timespec CMI::getSimTime(){
	return TimeUtil::Micros(Scratch::getDuration());

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
	if (t->isFinished()){
		ThermalApproachAPI::finishJob(t);
	}else{
		BusyWait* b = (BusyWait*)t;
		insertJobToQueue(b->getNextCoreId(),t);
	}
	
}


// Interface function to get member 'initialized'
bool CMI::isInitialized(){
	return initialized;
}
// Interface function to get member 'simulating'
bool CMI::isRunning(){
	return running;
}



unsigned CMI::getNCPU(){
	return n_cpus;
}


void CMI::saveResults(){
	if (Scratch::isSaveFile()){
		Statistics::toFile(Scratch::getName());
		cout << "saving..." << endl;

		string tempSaveName = Scratch::getName() + "_result.csv";

		vector<string> beginOfData = vector<string>(1, "111111111111111111111111111111");
		

		if (_isAppendSaveFile > 0){
			appendContentToFile(tempSaveName, beginOfData);
		}else{
			saveContentToNewFile(tempSaveName, beginOfData);
		}
		
		// appendToFile(tempSaveName, tempwatcher->getMeanTemp());
		// double maxTemp = tempwatcher->getMaxTemp();
		// appendToFile(tempSaveName, vector<double>(1, maxTemp));

		// double MeanMaxTemp = tempwatcher->getMeanMaxTemp();
		// appendToFile(tempSaveName, vector<double>(1, MeanMaxTemp));

		// appendToFile(tempSaveName, scheduler->getKernelTime());

		float total_cpu_usage = cpuUsageRecorder.getUsage();
		appendToFile(tempSaveName, vector<float>(1, total_cpu_usage));

		// appendContentToFile(tempSaveName, Statistics::getAllMissedDeadline());

		vector<string> endOfData = vector<string>(1, "999999999999999999999999999999");
		appendContentToFile(tempSaveName, endOfData);
		// appendToFile(tempSaveName, scheduler->getAllSchemes());
	}
}