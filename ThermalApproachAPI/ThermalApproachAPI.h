#ifndef THERMALAPPROACHAPI_H
#define THERMALAPPROACHAPI_H 

#include <string>
#include "utils/Enumerations.h"
#include "core/Task.h"


class ThermalApproachAPI{
private:
	static std::string approachName;

public:

	static void init(std::string _name);
	
	static Configuration runThermalApproach(const DynamicInfo&);

	static Configuration addNewJob(_task_type, const DynamicInfo&);

	static Configuration finishJob(Task t);

};



#endif
