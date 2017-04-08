#include "ThermalApproachAPI.h"

#include <iostream>

#include "core/CMI.h"
#include "results/Statistics.h"
#include "configuration/StateTable.h"
#include "configuration/Scratch.h"
#include "configuration/JobQueue.h"
#include "utils/TimeUtil.h"


using namespace std;

string ThermalApproachAPI::approachName = "default";

vector<unsigned long> ThermalApproachAPI::tons ;
vector<unsigned long> ThermalApproachAPI::toffs ;


//fill the following functions according to the thermal approach
void ThermalApproachAPI::init(string __aproachname){
	approachName = __aproachname;
	tons = TimeUtil::convert_us(Scratch::getPBOOTons());
	toffs = TimeUtil::convert_us(Scratch::getPBOOToffs());
}

Configuration ThermalApproachAPI::runThermalApproach(const DynamicInfo& info){
	Configuration ret;

	if (approachName == "example1"){

	}else if (approachName == "default"){

	}
	for (int i = 0; i < (int)tons.size(); ++i)
	{
		StateTable st = StateTable(i);
		st.pushState(0, toffs[i]);
		st.pushState(1500, tons[i]);
		JobQueue jq = JobQueue();

		ret.pushConfigurationOfOneCore(st, jq);
	}
	

	return ret;
}


int ThermalApproachAPI::addNewJob(_task_type, CMI* cmi){
	DynamicInfo p;
	// cmi->getDynamicInfo(p);

	return 0;

}


Configuration ThermalApproachAPI::finishJob(Task* t){
	Configuration ret;
	// cout << "Task with id: " << t->getId() << " finished at time: " << Statistics::getRelativeTime_ms() << endl; 
	return ret;
}