#include "dispatchers/PeriodicJitter.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "core/Processor.h"
#include "core/Worker.h"
#include "results/Statistics.h"
#include "utils/Operators.h"
#include "utils/TimeUtil.h"

using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** CONSTRUCTOR ***********/

PeriodicJitter::PeriodicJitter(unsigned int id) : Dispatcher(id)
{
   period      = TimeUtil::Millis(20);
   deltaPeriod = TimeUtil::Millis(5);

   //seed random numbers for calculating jitter
   srand(time(NULL));
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM DISPATCHER ****/

void PeriodicJitter::dispatch()
{
   struct timespec newPeriod, rem;
   double          random;

   while (Processor::isRunning())
   {
      // Statistics::addTrace(dispatcher, worker->getId(), task_arrival);

      if (processor != NULL)
      {
         Task *t = createNewTask();
         processor->newJob(t, TASK_TYPE);
      }
      else
      {
         cout << "Dispatcher error: worker is null!\n";
      }

      if (Processor::isRunning())
      {
         random    = (1 + (rand() % 200)) / 100.0; //random in [1/100,2]
         newPeriod = period + jitter - random * jitter - deltaPeriod;
         //deltaPeriod keeps the timing to n*P+-jitter
         deltaPeriod = jitter - random * jitter;
         nanosleep(&newPeriod, &rem);
      }
   }

  #if _INFO == 1
   cout << "Dispatcher " << id << " is exiting dispatch()\n";
  #endif
}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function returns the period
struct timespec PeriodicJitter::getPeriod()
{
   return(period);
}

///This function sets the jitter
void PeriodicJitter::setJitter(struct timespec j)
{
   jitter = j;
}

///This function sets the dispatcher's period
void PeriodicJitter::setPeriod(struct timespec p)
{
   period = p;
}
