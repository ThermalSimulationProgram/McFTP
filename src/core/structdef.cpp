#include "core/structdef.h"


#include "utils/utils.h"
#include "utils/TimeUtil.h"

#include <sstream>

using namespace std;

void pipeinfo_print(const pipeinfo & config){
	cout << "pipeinfo.adaptionIndex:  " <<config.adaptionIndex << endl;
	displayvector(config.Q, "pipeinfo.Q");
	displayvector(config.activeSet, "pipeinfo.activeSet");
	displayvector(config.sleepSet, "pipeinfo.sleepSet");
	displayvector(config.ccs, "pipeinfo.ccs");
	displayvector(config.dcs, "pipeinfo.dcs");
	displayvector(config.rho, "pipeinfo.rho");
	displayvector(config.K, "pipeinfo.K");
	displayvector(config.allT, "pipeinfo.allT");

	for (unsigned i = 0; i < config.FIFOcurveData.size(); ++i){
		stringstream field;
		field << "pipeinfo.FIFOcurveData" << i+1;
		displayvector(config.FIFOcurveData[i], field.str());
	}
	
}


