#include "ThermalApproachAPI.h"

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


Configuration ThermalApproachAPI::addNewJob(_task_type, const DynamicInfo&){

}

Configuration ThermalApproachAPI::finishJob(Task t){
	
}