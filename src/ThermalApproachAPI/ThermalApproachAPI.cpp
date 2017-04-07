#include "ThermalApproachAPI.h"

#include <iostream>

#include "core/CMI.h"
#include "results/Statistics.h"
#include "configuration/StateTable.h"
#include "configuration/JobQueue.h"


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

	StateTable st = StateTable(0);
	st.pushState(0, 20000);
	st.pushState(1500, 20000);
	JobQueue jq = JobQueue();

	ret.pushConfigurationOfOneCore(st, jq);

	StateTable st2 = StateTable(0);
	st2.pushState(0, 30000);
	st2.pushState(1500, 20000);

	ret.pushConfigurationOfOneCore(st2, jq);


	return ret;
}


int ThermalApproachAPI::addNewJob(_task_type, CMI* cmi){
	DynamicInfo p;
	// cmi->getDynamicInfo(p);

	return 0;

}

Configuration ThermalApproachAPI::finishJob(Task* t){
	Configuration ret;
	cout << "Task with id: " << t->getId() << " finished at time: " << Statistics::getRelativeTime_ms() << endl; 
	return ret;
}