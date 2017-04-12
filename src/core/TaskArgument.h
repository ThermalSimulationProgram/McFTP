#ifndef TASKARGUMENT_H
#define TASKARGUMENT_H 

#include <vector>
#include <string>
#include <time.h>

#include "utils/Enumerations.h"

// a simple data class, storing arguments specifying a task
class TaskArgument{
public:
	// necessary parameters
	_task_periodicity _periodicity;
	_task_load_type _load_type;
	_task_type _type;
	int taskId;
	std::string name;


	// parameters specifying the periodicity, 
	struct timespec period;
	struct timespec jitter;
	struct timespec release_time;

	
	// parameters specifying task mapping 
	std::vector<int> attached_cores;


	// parameters specifying task load
	int user_defined_load_id;

	int benchmark_id;

	std::vector<struct timespec> wcets;
	std::vector<unsigned long> wcets_us;


	TaskArgument();

	void print();

	int getLoadId();

};

#endif