#include "ThermalApproachAPI.h"

#include "core/CMI.h"
using namespace std;

string ThermalApproachAPI::approachName = "default";


//fill the following functions according to the thermal approach
void ThermalApproachAPI::init(string __aproachname){
	approachName = __aproachname;
}

Configuration ThermalApproachAPI::runThermalApproach(const DynamicInfo& info){
	Configuration ret;

	switch (approachName){
		case "example1":{

		}

		case "default":{
			break;
		}

	}

}


int ThermalApproachAPI::addNewJob(_task_type, CMI* cmi){
	DynamicInfo p;
	cmi->getDynamicInfo(p);

	return 0;

}

Configuration ThermalApproachAPI::finishJob(Task* t){
	
}