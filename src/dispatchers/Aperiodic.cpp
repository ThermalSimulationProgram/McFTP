#include "dispatchers/Aperiodic.h"

#include <iostream>

#include "core/Processor.h"
#include "results/Statistics.h"
#include "utils/Operators.h"
#include "utils/TimeUtil.h"
#include "configuration/Scratch.h"


using namespace std;

/***************************************
*        CLASS DEFINITION             *
***************************************/

/*********** CONSTRUCTOR ***********/

///Constructor needs a pointer to simulation and id
Aperiodic::Aperiodic(unsigned int id) : Dispatcher(id)
{
   releaseTime = TimeUtil::Millis(10); //default release time is 5 ms
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM DISPATCHER ****/

///This function was a flagged loop that activates the Worker according to the task periodicity
void Aperiodic::dispatch()
{
   struct timespec rem;

   nanosleep(&releaseTime, &rem);

   // #if _INFO == 1
   // cout << "+Worker " << worker->getId() << " has new job!\n";
   // #endif

   // Statistics::addTrace(dispatcher, worker->getId(), task_arrival);

   if (processor != NULL)
   {
      Task *t = createNewTask();
      processor->newJob(t, TASK_TYPE);
   }
   else
   {
      cout << "Dispatcher error: Processor is null!\n";
   }

   //wait until simulation is done to free worker
   do
   {
      struct timespec aux = TimeUtil::Micros(Scratch::getDuration()) - releaseTime - offset;
      nanosleep(&aux, &rem);
   } while (Processor::isRunning());
}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function returns the release time
struct timespec Aperiodic::getReleaseTime()
{
   return(releaseTime);
}

///This function sets the release time for the aperiodic dispatcher
void Aperiodic::setReleaseTime(struct timespec r)
{
   releaseTime = r;
}
