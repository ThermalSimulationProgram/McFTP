#include "dispatchers/Periodic.h"

#include "core/Simulation.h"
#include "core/Worker.h"
#include "results/Statistics.h"
#include "util/Operators.h"
#include "util/TimeUtil.h"

#include <iostream>
#include <stdio.h>

using namespace std;

#define _INFO 0

/***************************************
 *        CLASS DEFINITION             * 
 ***************************************/

/*********** CONSTRUCTOR ***********/

Periodic::Periodic(unsigned int id) : Dispatcher(id) {
  period =  TimeUtil::Millis(20);
}

/*********** INHERITED FUNCTIONS ***********/

/**** FROM DISPATCHER ****/  

void Periodic::dispatch() {
  struct timespec rem;

  while (Simulation::isSimulating()) {

    Statistics::addTrace(dispatcher, worker->getId(), task_arrival);

    if(worker != NULL) {
      #if _INFO==1
      cout << "Disp : " << id << " is registering a new job\n";// @t=" << TimeUtil::getTimeUSec() << "\n";
      #endif
      worker->newJob();
    }
    else {
      cout << "Dispatcher error: worker is null!\n";
    }

    if(Simulation::isSimulating()) {
      nanosleep(&period, &rem);
    }
  }

  #if _INFO == 1
  cout << "Dispatcher " << id << " is exiting dispatch()\n";
  #endif
}

/*********** GETTER AND SETTER FUNCTIONS ***********/

///This function returns the period
struct timespec Periodic::getPeriod() {
  return period;
}

///This function sets the dispatcher's period
void Periodic::setPeriod(struct timespec p) {
  period = p;
}

