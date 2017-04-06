#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <vector>

#include "configuration/StateTable.h"
#include "configuration/JobQueue.h"

class Configuration{
private:
	std::vector<StateTable> statetables;

	std::vector<JobQueue> jobqueues;
public:
	///COnstructor needs nothing
	Configuration();

	void pushConfigurationOfOneCore(const StateTable& s, const JobQueue& q);

	std::vector<StateTable> getAllStateTables();

	std::vector<JobQueue> getAllJobQueue();
	JobQueue getJobQueue(int id);

};





#endif