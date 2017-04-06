#include "ThermalApproachAPI.h"

#include <iostream>

#include "core/CMI.h"
#include "results/Statistics.h"
using namespace std;

string ThermalApproachAPI::approachName = "default";


//fill the following functions according to the thermal approach
void ThermalApproachAPI::init(string __aproachname){
	approachName = __aproachname;
}

Configuration ThermalApproachAPI::runThermalApproach(const DynamicInfo& info){
	Configuration ret;

	if (approachName == "example1"){

	}else if (approachName == "default"){

	}
	
	return ret;
}


int ThermalApproachAPI::addNewJob(_task_type, CMI* cmi){
	DynamicInfo p;
	cmi->getDynamicInfo(p);

	return 0;

}

Configuration ThermalApproachAPI::finishJob(Task* t){
	Configuration ret;
	cout << "Task with id: " << t->getId() << "finished at time: " << Statistics::getRelativeTime() << endl; 
	return ret;
}