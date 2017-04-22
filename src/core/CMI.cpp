#include "core/CMI.h"

#include <iostream>

#include "core/Processor.h"
#include "core/Task.h"
#include "configuration/Scratch.h"
#include "results/Statistics.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"
#include "utils/Enumerations.h"
#include "utils/vectormath.h"

	
using namespace std;

CMI::CMI(string xml_path){
	// construct the prcessor, including all workers, dispatchers, power manager, temperature watcher, etc.
	processor = new Processor(xml_path);

	offlineApproach = NULL;

	// default: no online thermal approach
	validApproach = NULL;
	onlineApproachId = -1;

	// default: no dynamic task allocator
	taskAllocator = NULL;

	// create the static task allocator
	// firstly, get all the task indexes
	vector<int> allTaskIds = processor->getAllTaskIds();
	// then, find the maximal task index, use it as the size of the static task allocator
	int maxTaskId = maxElement(allTaskIds);
	// create the static task allocator, default value -1 means invalid task index
	staticTaskAllocator = vector<int>(maxTaskId+1, -1);
	// set default task allocator
	// The tasks are allocated in a circular manner 
	workerNumber = processor->getWorkerNumber();
	// The workers are indexed from 0 to workerNumber-1
	int workerId = 0; //
	for (int i = 0; i < (int) allTaskIds.size(); ++i){		
		staticTaskAllocator[allTaskIds[i]] = workerId;
		++workerId;
		if ( workerId >= workerNumber){
			// switch back to the first worker
			workerId = 0;
		}
	}


}

CMI::~CMI(){
	delete processor;
}

void CMI::setOfflineThermalApproach(offline_thermal_approach _approach){
	offlineApproach = _approach;
}

void CMI::addOnlineThermalApproach(std::string _name, 
	online_thermal_approach _approach){

	online_thermal_approach_info_t new_info = {_name, _approach};
	onlineApproaches.push_back(new_info);
}

void CMI::setValidOnlineThermalApproach(std::string _name){

	bool found = false;
	for (int i = 0; i < (int)onlineApproaches.size(); ++i){
		if (onlineApproaches[i].name == _name){
			onlineApproachId = i;
			found = true;
			break;
		}
	}

	if (!found){
		cout << "CMI::setValidOnlineThermalApproach: given approach not found " << endl;
		exit(1);
	}else{
		validApproach = onlineApproaches[onlineApproachId].func;
	}

}


void CMI::printAllTaskInfo(){
	Scratch::printAllTaskInfo();
}

unsigned long CMI::getRelativeTimeUsec(){
	if (processor->isRunning()){
		return TimeUtil::getTimeUSec();
	}else{
		return 0;
	}
	
}

void CMI::setOnlineThermalApproachPeriod(unsigned long period_us){
	processor->setThermalApproachPeriod(period_us);
}

void CMI::setTaskRunningCore(int _taskId, int _workerId){
	if (  ( _taskId < 0)    ||    ( _taskId >= (int)staticTaskAllocator.size() )    || 
		staticTaskAllocator[_taskId] < 0){
		cout << "CMI::setTaskRunningCore: error! Input taskId: "<< _taskId << " does not exist" << endl;
		exit(1);
	}
	if (_workerId < 0 || _workerId >= workerNumber ){
		cout << "CMI::setTaskRunningCore: error! Input workerId: "<< _workerId << " does not exist" << endl;
		exit(1);
	}

	staticTaskAllocator[_taskId] = _workerId;

}
void CMI::setOnlineTaskAllocator(online_task_allocator _allocator){
	taskAllocator = _allocator;
}

void CMI::startRunning(){
	processor->setCMI(this);
	processor->initialize();
	processor->simulate();
}


// This function is called by the dispatcher to release a new job
int CMI::getNewJobTargetCore(Task * t, _task_type type){
	// if (t->getType() == pipelined ){
	// 	return 0;
	// }else{
	// 	return t->getTaskId();
	// }

	if (taskAllocator == NULL){
		return staticTaskAllocator[t->getTaskId()];
	}else{
		int coreId = taskAllocator(t->getTaskId());
		if (coreId < 0 || coreId >= workerNumber){
			cout << "CMI::getNewJobTargetCore: Given dynamic task allocator returns an invalid core index. "
			<< "Revise it to 0! " << endl;
			coreId = 0;
		}
		return coreId;
	}

}

// This function is called by the work when _task has been finished
void CMI::finishedJob(Task* _task){

}


void CMI::getDynamicInfo(DynamicInfo& p){
	processor->getDynamicInfo(p);
}

unsigned long CMI::getCurrentSimTime_ms(){
	return Statistics::getRelativeTime_ms();
}


void CMI::lockAllJobs(){
	processor->lockTaskQueues();
	processor->lockAllWorkers();
}

void CMI::unlockAllJobs(){
	processor->unlockTaskQueues();
	processor->unlockAllWorkers();
}

vector<int> CMI::getAllTaskIds(){
	return processor->getAllTaskIds();
}

