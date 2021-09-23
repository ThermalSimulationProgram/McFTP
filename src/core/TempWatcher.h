#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H


#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <vector>
#include <string>
#include <semaphore.h>

#include <time.h>

#include "pthread/Thread.h"
#include "performance_counter/PerformanceCounters.h"
#include "CMIFunctionPointerDefs.h"

#include "../McFTPBuildConfig.h"


class Processor;

class TempWatcher : public Thread {
protected:

   std::vector <double> curHardwareTemp;

   std::vector <double> curSoftTemp;


   bool useHardwareSensor;

   bool useSoftSensor;

   struct timespec samplingPeriod;

   sem_t temp_sem;

   // In the case of using soft temperature sensor,
   // mcftp will call the default linear soft temperature calculator to get the temperature
   // for each given performance counter, if this variable is set to NULL
   // Otherwise, this function will be called to obtain the temperatures according to
   // the values of the performance counters
   user_defined_soft_temperature_sensor softSensor;

   Processor *processor;
public:
   TempWatcher(unsigned, unsigned, Processor *);

   TempWatcher(unsigned, unsigned, bool, bool, Processor *);

   ~TempWatcher();

   void activate();

   ///This join function takes into account the dispatcher's unblocking mechanism
   void join();

   void wrapper();

   bool isSoftSensorEnabled();

   std::vector <double> get_cpu_temperature();

   std::vector <double> get_soft_cpu_temperature();

   std::vector <double> getCurHardwareTemp();

   std::vector <double> getCurSoftwareTemp();
};



#endif
