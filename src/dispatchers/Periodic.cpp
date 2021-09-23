#include "dispatchers/Periodic.h"

#include "core/Processor.h"
#include "core/Worker.h"
#include "results/Statistics.h"
#include "utils/Operators.h"
#include "utils/TimeUtil.h"

#include <iostream>
#include <stdio.h>

using namespace std;

#define _INFO    0

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** CONSTRUCTOR ***********/

Periodic::Periodic(unsigned int id) : Dispatcher(id)
{
   period = TimeUtil::Millis(20);
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM DISPATCHER ****/

void Periodic::dispatch()
{
   struct timespec rem;



   // #if _INFO==1
   //     cout << "Inside Periodic::dispatch " << endl;// @t=" << TimeUtil::getTimeUSec() << "\n";
   //     cout << "period is: " << TimeUtil::convert_ms(period) << endl;
   // #endif
   if (processor == NULL)
   {
      cout << "Dispatcher error: cmi is null!\n";
      return;
   }
   struct timespec now = TimeUtil::getTime();

   while (Processor::isRunning())
   {
      // Statistics::addTrace(dispatcher, worker->getId(), task_arrival);


      #if _INFO == 1
      cout << "Disp : " << id << " is registering a new job\n";// @t=" << TimeUtil::getTimeUSec() << "\n";
      #endif
      Task *t = createNewTask();
      processor->newJob(t, TASK_TYPE);

      rem = now + period;
      sem_timedwait(&dispatch_sem, &rem);
      now = rem;
      // nanosleep(&period, &rem);
   }

  #if _INFO == 1
   cout << "Dispatcher " << id << " is exiting dispatch()\n";
  #endif
}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function returns the period
struct timespec Periodic::getPeriod()
{
   return(period);
}

///This function sets the dispatcher's period
void Periodic::setPeriod(struct timespec p)
{
   period = p;
}
