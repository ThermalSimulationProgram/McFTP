#include "Aperiodic.h"

#include "CMI.h"
#include "results/Statistics.h"
#include "util/Operators.h"
#include "util/TimeUtil.h"

#include <iostream>

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

///Constructor needs a pointer to simulation and id
Aperiodic::Aperiodic(unsigned int id) : Dispatcher(id) {
  releaseTime = TimeUtil::Millis(5); //default release time is 5 ms
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM DISPATCHER ****/

///This function was a flagged loop that activates the Worker according to the task periodicity
void Aperiodic::dispatch() {
  struct timespec rem;

  nanosleep(&releaseTime, &rem);

  #if _INFO == 1
  cout << "+Worker " << worker->getId() << " has new job!\n";
  #endif  

  Statistics::addTrace(dispatcher, worker->getId(), task_arrival);
  
  if(cmi != NULL) {
    cmi->newJob();
  }
  else {
    cout << "Dispatcher error: CMI is null!\n";
  }
  
  //wait until simulation is done to free worker
  do {
    struct timespec aux = Simulation::getSimTime() - releaseTime - offset;
    nanosleep(&aux, &rem);
  } while(Simulation::isSimulating());
  

}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function returns the release time
struct timespec Aperiodic::getReleaseTime() {
  return releaseTime;
}

///This function sets the release time for the aperiodic dispatcher
void Aperiodic::setReleaseTime(struct timespec r) {
  releaseTime = r;
}
