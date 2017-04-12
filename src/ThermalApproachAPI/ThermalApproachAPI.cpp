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
		// st.pushState(0, toffs[i]);
		st.pushState(3400000, tons[i]);

		st.pushState(0, toffs[i]);
		st.pushState(800000, tons[i]);
		st.pushState(2100000, tons[i]);
		st.pushState(3200000, tons[i]);
		st.pushState(0, tons[i]);
		st.pushState(3400000, tons[i]);



		JobQueue jq = JobQueue();

		ret.pushConfigurationOfOneCore(st, jq);
	}
	

	return ret;
}


int ThermalApproachAPI::taskAllocator(Task *t, _task_type type, CMI* cmi){
	/*unsigned long time_ms =   cmi->getCurrentSimTime_ms();            
	if ( time_ms < 1000000 ){
		if (t->getTaskId() == 0)
			return 0;
		else
			return 1;

	}else if (time_ms < 2000000){ 
		if (t->getTaskId() == 0)
			return 1;
		else
			return 0;

	} else if (time_ms < 3000000){
		if (t->getTaskId() == 0)
			return 2;
		else
			return 3;

	}else{
		if (t->getTaskId() == 0)
			return 3;
		else
			return 2;
	}*/

	return t->getTaskId();

}


void ThermalApproachAPI::finishJob(Task* t, CMI* cmi){
	// std::cout << "task with id: " << t->getId() << " finishes at time: "
	// << cmi->getCurrentSimTime_ms() << " millisecond!\n";	
}