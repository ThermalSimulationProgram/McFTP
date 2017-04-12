#ifndef THERMALAPPROACHAPI_H
#define THERMALAPPROACHAPI_H 

#include <string>
#include <vector>

#include "utils/Enumerations.h"
#include "core/Task.h"
#include "configuration/Configuration.h"
#include "core/structdef.h"

class CMI;


class ThermalApproachAPI{
private:
	static std::string approachName;

	static std::vector<unsigned long> tons;
	static std::vector<unsigned long> toffs;

public:

	static void init(std::string _name);
	
	static Configuration runThermalApproach(const DynamicInfo&);

	static int taskAllocator(Task *t, _task_type, CMI* cmi);

	static void finishJob(Task* t, CMI* cmi);

};



#endif
