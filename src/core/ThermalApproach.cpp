#include "ThermalApproach.h"

#include <iostream>

#include "pthread/Priorities.h"
#include "utils/Semaphores.h"
#include "configuration/Scratch.h"
#include "configuration/StateTable.h"
#include "core/Processor.h"
#include "core/structdef.h"
#include "utils/TimeUtil.h"
#include "utils/Operators.h"


using namespace std;

#define _INFO     1
#define _DEBUG    0;

ThermalApproach::ThermalApproach(CMI *_cmi, unsigned _id, Processor *c,
                                 string _approachName) : Thread(_id)
{
   cmi = _cmi;
   ///  initialize member vairables
   processor    = c;
   thread_type  = _thermal_approach;
   approachName = _approachName;

   /// set thermal approach period
   period = TimeUtil::Micros(Scratch::getAdaptionPeriod());

   timeExpense.reserve(1000);
}

ThermalApproach::~ThermalApproach()
{
}

void ThermalApproach::init()
{
   // all initialized, post the semaphore to signal main thread scheduler is ready
   Semaphores::rtcinit_sem.post_sem();
}

void ThermalApproach::activate()
{
   setPriority(Priorities::get_tempwatcher_pr());
}

void ThermalApproach::join()
{
   join2();
}

void ThermalApproach::wrapper()
{
   init();
   struct timespec rem;

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "ThermalApproach: " << id << " waiting for initialization\n";
   Semaphores::print_sem.post_sem();
        #endif

   cout << "ThermalApproach: period: " << TimeUtil::convert_us(period) << " us" << endl;

   //Wait until the simulation is initialized
   sem_wait(&Processor::init_sem);

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "ThermalApproach: " << id << " wating for execution \n";
   Semaphores::print_sem.post_sem();
        #endif

   ///wait for the simulation start
   sem_wait(&Processor::running_sem);

        #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "ThermalApproach: " << id << " begin execution \n";
   Semaphores::print_sem.post_sem();
        #endif


   vector <StateTable> stables;
   cmi->runOfflineApproach(stables);
   processor->updateStateTables(stables);

   while (Processor::isRunning() && cmi->isOnlineApproach())
   {
      DynamicInfo dinfo;
      processor->getDynamicInfo(dinfo);

      struct timespec     timein = TimeUtil::getTime();
      vector <StateTable> stables;
      cmi->runOnlineApproach(dinfo, stables);
      struct timespec timeout = TimeUtil::getTime();

      processor->updateStateTables(stables);

      timeExpense.push_back(TimeUtil::convert_us(timeout - timein));

      nanosleep(&period, &rem);
   }


   // #if _INFO == 1
   Semaphores::print_sem.wait_sem();
   cout << "ThermalApproach " << id << " exiting wrapper...\n";
   Semaphores::print_sem.post_sem();
   // #endif
}

vector <double> ThermalApproach::getKernelTime()
{
   return(timeExpense);
}

void ThermalApproach::setPeriod(unsigned long period_us)
{
   period = TimeUtil::Micros(period_us);
}
