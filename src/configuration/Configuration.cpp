#include "configuration/Configuration.h"

#include <iostream>
#include <cstdlib>

using namespace std;
Configuration::Configuration(){

}


void Configuration::pushConfigurationOfOneCore(
	const StateTable& s, 
	const JobQueue& q){

	statetables.push_back(s);
	jobqueues.push_back(q);
}


vector<StateTable> Configuration::getAllStateTables() const{
	return statetables;
}

vector<JobQueue> Configuration::getAllJobQueue() const{
	return jobqueues;
}

JobQueue Configuration::getJobQueue(int id){
	if (id < 0 || id >= (int)jobqueues.size()){
		cerr << "Configuration::getJobQueue: invalid index" << endl;
		exit(1);
	}
	return jobqueues[id];
}


