#ifndef MCFTP_CMIFUNCTIONPOINTERDEFS_H
#define MCFTP_CMIFUNCTIONPOINTERDEFS_H

#include <vector>

#include "core/structdef.h"
#include "configuration/StateTable.h"

class CMI;

typedef void (*online_thermal_approach) (CMI *cmi, const DynamicInfo& p, std::vector <StateTable>& c);

typedef void (*offline_thermal_approach) (CMI *cmi, std::vector <StateTable>& c);

typedef int (*online_task_allocator)(CMI *cmi, int _taskId);

typedef std::vector <double> (*user_defined_soft_temperature_sensor)(
   std::vector <long long>& performanceCounterValues
   );

typedef std::vector <double> (*soft_temperature_sensor_power_estimator)(
   std::vector <long long>& performanceCounterValues
   );

#endif
