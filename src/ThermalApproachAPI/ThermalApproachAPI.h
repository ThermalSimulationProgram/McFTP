#ifndef THERMALAPPROACHAPI_H
#define THERMALAPPROACHAPI_H 

#include <string>

#include "utils/Enumerations.h"
#include "core/Task.h"
#include "configuration/Configuration.h"
#include "core/structdef.h"

class CMI;


class ThermalApproachAPI{
private:
	static std::string approachName;

public:

	static void init(std::string _name);
	
	static Configuration runThermalApproach(const DynamicInfo&);

	static int addNewJob(_task_type, CMI* cmi);

	static Configuration finishJob(Task* t);

};



#endif
