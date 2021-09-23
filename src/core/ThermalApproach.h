#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <vector>
#include <string>
#include <time.h>

#include <semaphore.h>

#include "pthread/Thread.h"
#include "CMI.h"



class Processor;

class ThermalApproach : public Thread
{
protected:

   /////////////////////////////////////////////
   struct timespec period;

   Processor *processor;

   std::string approachName;

   std::vector <double> timeExpense;

   CMI *cmi;

public:
   ThermalApproach(CMI *cmi, unsigned _id, Processor *c, std::string name);

   ~ThermalApproach();

   void init();

   void activate();

   void wrapper();

   void join();

   std::vector <double> getKernelTime();

   void setPeriod(unsigned long period_us);
};



#endif
